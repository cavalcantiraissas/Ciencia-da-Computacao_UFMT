/*
 * opzip_por_posicao.c — variante experimental usada APENAS para o
 * experimento de contencao do relatorio (nao e a solucao entregue).
 *
 * Em vez de um unico mutex global protegendo todo o buffer circular
 * (como em opzip.c), esta variante usa um mutex + condition variable
 * dedicados a CADA posicao do buffer, e um mutex + condition variable
 * separados apenas para a variavel de progresso (proximo_a_escrever).
 * Isso permite que produtoras escrevendo em posicoes diferentes do
 * buffer nao disputem o mesmo lock entre si.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int indice_bloco;
    char *dados_comprimidos;
    size_t tamanho_comprimido;
    int pronto;
    pthread_mutex_t mutex;
    pthread_cond_t cond_pronto;
} Slot;

static char *arquivo_dados = NULL;
static size_t arquivo_tamanho = 0;
static size_t tam_bloco = 0;
static int num_blocos = 0;

static int proximo_bloco = 0;
static pthread_mutex_t mutex_bloco = PTHREAD_MUTEX_INITIALIZER;

static Slot *buffer = NULL;
static int K = 0;

/* Mutex + condition variable dedicados apenas ao controle da janela
 * deslizante (proximo_a_escrever), independentes dos locks de cada slot. */
static int proximo_a_escrever = 0;
static pthread_mutex_t mutex_progresso = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_progresso = PTHREAD_COND_INITIALIZER;

static FILE *saida = NULL;

static char *comprimir_bloco(const char *dados, size_t tamanho, size_t *tamanho_saida) {
    size_t capacidade = (tamanho == 0) ? 1 : tamanho * 5;
    char *saida_buf = malloc(capacidade);
    if (saida_buf == NULL) {
        fprintf(stderr, "opzip: falha ao alocar memoria para compressao\n");
        exit(1);
    }
    size_t pos = 0, i = 0;
    while (i < tamanho) {
        char c = dados[i];
        size_t count = 1;
        while (i + count < tamanho && dados[i + count] == c) count++;
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
        int idx = proximo_bloco++;
        pthread_mutex_unlock(&mutex_bloco);

        size_t offset = (size_t) idx * tam_bloco;
        size_t tamanho = ((offset + tam_bloco) <= arquivo_tamanho)
                              ? tam_bloco
                              : (arquivo_tamanho - offset);

        size_t tamanho_comp;
        char *comp = comprimir_bloco(arquivo_dados + offset, tamanho, &tamanho_comp);

        /* Espera a janela deslizante liberar espaco para este indice
         * (mutex separado dos locks de slot). */
        pthread_mutex_lock(&mutex_progresso);
        while (idx >= proximo_a_escrever + K) {
            pthread_cond_wait(&cond_progresso, &mutex_progresso);
        }
        pthread_mutex_unlock(&mutex_progresso);

        int posicao = idx % K;
        pthread_mutex_lock(&buffer[posicao].mutex);
        buffer[posicao].indice_bloco = idx;
        buffer[posicao].dados_comprimidos = comp;
        buffer[posicao].tamanho_comprimido = tamanho_comp;
        buffer[posicao].pronto = 1;
        pthread_cond_signal(&buffer[posicao].cond_pronto);
        pthread_mutex_unlock(&buffer[posicao].mutex);
    }
    return NULL;
}

static void *thread_consumidora(void *arg) {
    (void) arg;
    while (proximo_a_escrever < num_blocos) {
        int posicao = proximo_a_escrever % K;

        pthread_mutex_lock(&buffer[posicao].mutex);
        while (!(buffer[posicao].pronto && buffer[posicao].indice_bloco == proximo_a_escrever)) {
            pthread_cond_wait(&buffer[posicao].cond_pronto, &buffer[posicao].mutex);
        }
        char *dados = buffer[posicao].dados_comprimidos;
        size_t tamanho = buffer[posicao].tamanho_comprimido;
        buffer[posicao].pronto = 0;
        buffer[posicao].dados_comprimidos = NULL;
        pthread_mutex_unlock(&buffer[posicao].mutex);

        if (tamanho > 0) {
            fwrite(dados, 1, tamanho, saida);
        }
        free(dados);

        pthread_mutex_lock(&mutex_progresso);
        proximo_a_escrever++;
        pthread_cond_broadcast(&cond_progresso);
        pthread_mutex_unlock(&mutex_progresso);
    }
    return NULL;
}

static size_t tamanho_do_arquivo(FILE *f) {
    if (fseek(f, 0, SEEK_END) != 0) { perror("opzip: fseek"); exit(1); }
    long tam = ftell(f);
    if (tam < 0) { perror("opzip: ftell"); exit(1); }
    rewind(f);
    return (size_t) tam;
}

static void uso(const char *prog) {
    fprintf(stderr, "uso: %s [-t N] [-b TAM_BLOCO] [-k K] arquivo > arquivo.oz\n", prog);
}

int main(int argc, char *argv[]) {
    long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
    int num_threads = (ncpu > 0) ? (int) ncpu : 1;
    tam_bloco = 1024 * 1024;
    K = -1;

    int opt;
    while ((opt = getopt(argc, argv, "t:b:k:")) != -1) {
        switch (opt) {
            case 't': num_threads = atoi(optarg); if (num_threads < 1) return 1; break;
            case 'b': { long b = atol(optarg); if (b < 1) return 1; tam_bloco = (size_t) b; break; }
            case 'k': K = atoi(optarg); if (K < 1) return 1; break;
            default: uso(argv[0]); return 1;
        }
    }
    if (optind >= argc) { uso(argv[0]); return 1; }
    const char *caminho_arquivo = argv[optind];
    if (K == -1) K = 2 * num_threads;

    FILE *entrada = fopen(caminho_arquivo, "rb");
    if (entrada == NULL) { fprintf(stderr, "opzip: nao foi possivel abrir '%s'\n", caminho_arquivo); return 1; }
    arquivo_tamanho = tamanho_do_arquivo(entrada);
    if (arquivo_tamanho > 0) {
        arquivo_dados = malloc(arquivo_tamanho);
        if (arquivo_dados == NULL) { fclose(entrada); return 1; }
        if (fread(arquivo_dados, 1, arquivo_tamanho, entrada) != arquivo_tamanho) {
            fclose(entrada); free(arquivo_dados); return 1;
        }
    }
    fclose(entrada);

    num_blocos = (arquivo_tamanho == 0) ? 0 : (int) ((arquivo_tamanho + tam_bloco - 1) / tam_bloco);
    saida = stdout;

    if (num_blocos == 0) { free(arquivo_dados); return 0; }
    if (num_threads > num_blocos) num_threads = num_blocos;

    buffer = calloc((size_t) K, sizeof(Slot));
    if (buffer == NULL) { free(arquivo_dados); return 1; }
    for (int i = 0; i < K; i++) {
        pthread_mutex_init(&buffer[i].mutex, NULL);
        pthread_cond_init(&buffer[i].cond_pronto, NULL);
    }

    pthread_t *produtoras = malloc((size_t) num_threads * sizeof(pthread_t));
    pthread_t consumidora;
    pthread_create(&consumidora, NULL, thread_consumidora, NULL);
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&produtoras[i], NULL, thread_produtora, NULL);
    }
    for (int i = 0; i < num_threads; i++) {
        pthread_join(produtoras[i], NULL);
    }
    pthread_join(consumidora, NULL);
    fflush(saida);

    for (int i = 0; i < K; i++) {
        pthread_mutex_destroy(&buffer[i].mutex);
        pthread_cond_destroy(&buffer[i].cond_pronto);
    }
    free(produtoras);
    free(buffer);
    free(arquivo_dados);
    return 0;
}
