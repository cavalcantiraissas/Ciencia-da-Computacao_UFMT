/*
 * opzip.c — Zip paralelo com escrita ordenada (RLE)
 *
 * Pipeline produtor-consumidor: threads produtoras comprimem blocos do
 * arquivo de entrada em paralelo (distribuicao dinamica via indice
 * compartilhado); uma thread consumidora dedicada escreve os blocos
 * comprimidos no arquivo de saida, na ordem original, atraves de um
 * buffer circular de K posicoes sincronizado com mutex + condition
 * variables (sem busy-waiting).
 *
 * Uso: ./opzip [-t N] [-b TAM_BLOCO] [-k K] arquivo > arquivo.oz
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int indice_bloco; // Qual é o número do bloco (ex: Bloco 0, Bloco 1....)
    char *dados_comprimidos; // Ponteiro para os dados comprimidos na memória RAM
    size_t tamanho_comprimido; // Tamanho (em bytes) do bloco comprimido
    int pronto; // Flag (0 ou 1) indicando se o bloco está pronto para ser escrito
} Slot;

/* 
Como as threads produtoras comprimem blocos de tamanhos diferentes em tempos diferentes,
os blocos podem ficar prontos fora de ordem. O indice_bloco permite que a thread consumidora
valide se o dado contido ali é realemnte o próximo blloco que ela precisa escrever no arquivo
*/

/* Dados de entrada (somente leitura apos carregamento)*/
static char *arquivo_dados = NULL;
static size_t arquivo_tamanho = 0;
static size_t tam_bloco = 0;
static int num_blocos = 0;

/* Distribuicao dinamica de blocos */
static int proximo_bloco = 0;
static pthread_mutex_t mutex_bloco = PTHREAD_MUTEX_INITIALIZER;

/* Buffer circular compartilhado */
static Slot *buffer = NULL;
static int K = 0;
static pthread_mutex_t mutex_buffer = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_slot_livre = PTHREAD_COND_INITIALIZER;   /* produtoras esperam aqui */
static pthread_cond_t cond_slot_pronto = PTHREAD_COND_INITIALIZER; /* consumidora espera aqui */

static int proximo_a_escrever = 0;
static FILE *saida = NULL;

/* Comprime um bloco de memoria usando run-length encoding.
 * Cada run e gravada como 4 bytes (count, little-endian, int32_t) +
 * 1 byte (caractere), via dois fwrite/memcpy separados para evitar
 * padding de struct. Retorna buffer alocado com malloc; quem chama
 * deve dar free(). */
static char *comprimir_bloco(const char *dados, size_t tamanho, size_t *tamanho_saida) {
    size_t capacidade = (tamanho == 0) ? 1 : tamanho * 5;
    char *saida_buf = malloc(capacidade);
    if (saida_buf == NULL) {
        fprintf(stderr, "opzip: falha ao alocar memoria para compressao\n");
        exit(1);
    }

    size_t pos = 0;
    size_t i = 0;
    while (i < tamanho) {
        char c = dados[i];
        size_t count = 1;
        while (i + count < tamanho && dados[i + count] == c) {
            count++;
        }
        int32_t cnt = (int32_t) count;
        memcpy(saida_buf + pos, &cnt, sizeof(int32_t));
        pos += sizeof(int32_t);
        saida_buf[pos] = c;
        pos += 1;
        i += count;
    }

    *tamanho_saida = pos;
    return saida_buf;
}

static void *thread_produtora(void *arg) {
    (void) arg;
    for (;;) {
        pthread_mutex_lock(&mutex_bloco);
        if (proximo_bloco >= num_blocos) {
            pthread_mutex_unlock(&mutex_bloco);
            break;
        }
        int idx = proximo_bloco;
        proximo_bloco++;
        pthread_mutex_unlock(&mutex_bloco);

        size_t offset = (size_t) idx * tam_bloco;
        size_t tamanho = ((offset + tam_bloco) <= arquivo_tamanho)
                              ? tam_bloco
                              : (arquivo_tamanho - offset);

        size_t tamanho_comp;
        char *comp = comprimir_bloco(arquivo_dados + offset, tamanho, &tamanho_comp);

        int posicao = idx % K;

        pthread_mutex_lock(&mutex_buffer);
        /* A posicao so pode ser considerada livre para o bloco idx quando
         * idx esta dentro da janela [proximo_a_escrever, proximo_a_escrever + K).
         * Checar apenas buffer[posicao].pronto nao basta: com distribuicao
         * dinamica de blocos, uma thread pode terminar de comprimir um
         * bloco de indice alto (ex.: idx = proximo_a_escrever + K) antes de
         * o bloco de indice mais baixo que ocupa a mesma posicao (idx - K)
         * sequer ter sido produzido; nesse caso a posicao aparenta livre
         * (pronto == 0) mas ainda nao e a vez desse bloco, e escrever nela
         * quebraria a ordem e poderia travar a consumidora permanentemente. */
        while (idx >= proximo_a_escrever + K) {
            pthread_cond_wait(&cond_slot_livre, &mutex_buffer);
        }
        buffer[posicao].indice_bloco = idx;
        buffer[posicao].dados_comprimidos = comp;
        buffer[posicao].tamanho_comprimido = tamanho_comp;
        buffer[posicao].pronto = 1;
        pthread_cond_broadcast(&cond_slot_pronto);
        pthread_mutex_unlock(&mutex_buffer);
    }
    return NULL;
}

static void *thread_consumidora(void *arg) {
    (void) arg;
    while (proximo_a_escrever < num_blocos) {
        int posicao = proximo_a_escrever % K;

        pthread_mutex_lock(&mutex_buffer);
        while (!(buffer[posicao].pronto && buffer[posicao].indice_bloco == proximo_a_escrever)) {
            pthread_cond_wait(&cond_slot_pronto, &mutex_buffer);
        }
        char *dados = buffer[posicao].dados_comprimidos;
        size_t tamanho = buffer[posicao].tamanho_comprimido;
        pthread_mutex_unlock(&mutex_buffer);

        if (tamanho > 0) {
            fwrite(dados, 1, tamanho, saida);
        }
        free(dados);

        pthread_mutex_lock(&mutex_buffer);
        buffer[posicao].pronto = 0;
        buffer[posicao].dados_comprimidos = NULL;
        proximo_a_escrever++;
        pthread_cond_broadcast(&cond_slot_livre);
        pthread_mutex_unlock(&mutex_buffer);
    }
    return NULL;
}

static size_t tamanho_do_arquivo(FILE *f) {
    if (fseek(f, 0, SEEK_END) != 0) {
        perror("opzip: fseek");
        exit(1);
    }
    long tam = ftell(f);
    if (tam < 0) {
        perror("opzip: ftell");
        exit(1);
    }
    rewind(f);
    return (size_t) tam;
}

static void uso(const char *prog) {
    fprintf(stderr, "uso: %s [-t N] [-b TAM_BLOCO] [-k K] arquivo > arquivo.oz\n", prog);
}

int main(int argc, char *argv[]) {
    long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    int num_threads = (ncpu > 0) ? (int) ncpu : 1;
    if (num_threads < 1) {
        num_threads = 1;
    }
    tam_bloco = 1024 * 1024; /* 1 MB default */
    K = -1; /* marcador: usar default (2 * num_threads) apos parse */

    int opt;
    while ((opt = getopt(argc, argv, "t:b:k:")) != -1) {
        switch (opt) {
            case 't':
                num_threads = atoi(optarg);
                if (num_threads < 1) {
                    fprintf(stderr, "opzip: -t deve ser >= 1\n");
                    return 1;
                }
                break;
            case 'b': {
                long b = atol(optarg);
                if (b < 1) {
                    fprintf(stderr, "opzip: -b deve ser >= 1\n");
                    return 1;
                }
                tam_bloco = (size_t) b;
                break;
            }
            case 'k':
                K = atoi(optarg);
                if (K < 1) {
                    fprintf(stderr, "opzip: -k deve ser >= 1\n");
                    return 1;
                }
                break;
            default:
                uso(argv[0]);
                return 1;
        }
    }

    if (optind >= argc) {
        uso(argv[0]);
        return 1;
    }
    const char *caminho_arquivo = argv[optind];

    if (K == -1) {
        K = 2 * num_threads;
    }

    /* 1. Leitura sequencial do arquivo de entrada para memoria */
    FILE *entrada = fopen(caminho_arquivo, "rb");
    if (entrada == NULL) {
        fprintf(stderr, "opzip: nao foi possivel abrir '%s'\n", caminho_arquivo);
        return 1;
    }

    arquivo_tamanho = tamanho_do_arquivo(entrada);

    if (arquivo_tamanho > 0) {
        arquivo_dados = malloc(arquivo_tamanho);
        if (arquivo_dados == NULL) {
            fprintf(stderr, "opzip: falha ao alocar memoria para o arquivo\n");
            fclose(entrada);
            return 1;
        }
        size_t lidos = fread(arquivo_dados, 1, arquivo_tamanho, entrada);
        if (lidos != arquivo_tamanho) {
            fprintf(stderr, "opzip: falha ao ler o arquivo de entrada\n");
            fclose(entrada);
            free(arquivo_dados);
            return 1;
        }
    }
    fclose(entrada);

    /* num_blocos = ceil(tamanho_arquivo / TAM_BLOCO) */
    num_blocos = (arquivo_tamanho == 0)
                     ? 0
                     : (int) ((arquivo_tamanho + tam_bloco - 1) / tam_bloco);

    saida = stdout;

    if (num_blocos == 0) {
        /* Arquivo vazio: nada a comprimir, nada a escrever. */
        free(arquivo_dados);
        return 0;
    }

    /* Nao faz sentido criar mais threads produtoras do que blocos. */
    if (num_threads > num_blocos) {
        num_threads = num_blocos;
    }

    /* 2. Inicializa buffer circular */
    buffer = calloc((size_t) K, sizeof(Slot));
    if (buffer == NULL) {
        fprintf(stderr, "opzip: falha ao alocar buffer circular\n");
        free(arquivo_dados);
        return 1;
    }

    /* 3. Dispara threads produtoras e a thread consumidora */
    pthread_t *produtoras = malloc((size_t) num_threads * sizeof(pthread_t));
    if (produtoras == NULL) {
        fprintf(stderr, "opzip: falha ao alocar threads\n");
        free(arquivo_dados);
        free(buffer);
        return 1;
    }

    pthread_t consumidora;
    pthread_create(&consumidora, NULL, thread_consumidora, NULL);

    for (int i = 0; i < num_threads; i++) {
        pthread_create(&produtoras[i], NULL, thread_produtora, NULL);
    }

    /* 4. Encerramento: junta produtoras e depois a consumidora */
    for (int i = 0; i < num_threads; i++) {
        pthread_join(produtoras[i], NULL);
    }
    pthread_join(consumidora, NULL);

    fflush(saida);

    free(produtoras);
    free(buffer);
    free(arquivo_dados);

    return 0;
}
