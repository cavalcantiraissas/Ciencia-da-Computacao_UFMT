/*
 * Pipeline de segurança de uma caldeira com 3 processos MPI.
 *
 *   [Rank 0: Sensor] --(MPI_Ssend: [id, temp])--> [Rank 1: Controlador]
 *   [Rank 1: Controlador] --(MPI_Bsend: [id, temp, acao])--> [Rank 2: Logger]
 *
 * - Sensor: gera 100 leituras aleatórias entre 50 e 200 °C, uma por segundo.
 *   O envio é síncrono (MPI_Ssend): só retorna quando o controlador já começou
 *   a receber a medição, então nenhuma leitura é feita antes disso.
 * - Controlador: temperatura > 100 °C abre a válvula de alívio (acao = 1),
 *   senão mantém fechada (acao = 0). Repassa o evento ao log com MPI_Bsend,
 *   num buffer anexado antes do laço com espaço para todos os eventos, de modo
 *   que um logger lento nunca trava o ciclo de controle.
 * - Logger: imprime cada evento no console e no arquivo caldeira_log.txt e,
 *   ao final, um resumo.
 *
 * Compilar: mpicc -O2 caldeira_mpi.c -o caldeira_mpi
 * Executar: mpirun -np 3 ./caldeira_mpi [leituras] [intervalo_ms]
 *           (padrão: 100 leituras, 1000 ms)
 */
#define _POSIX_C_SOURCE 199309L
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

#define SENSOR      0
#define CONTROLADOR 1
#define LOGGER      2

#define TAG_LEITURA 10
#define TAG_EVENTO  20

#define TEMP_MIN     50.0
#define TEMP_MAX    200.0
#define TEMP_LIMITE 100.0

#define ARQUIVO_LOG "caldeira_log.txt"

typedef struct {        /* Sensor -> Controlador */
    int    id;
    double temp;
} Leitura;

typedef struct {        /* Controlador -> Logger */
    int    id;
    double temp;
    int    acao;        /* 1 = válvula de alívio aberta, 0 = fechada */
} Evento;

/* Tipos derivados para enviar as structs numa única mensagem,
 * com o id e a ação como MPI_INT e a temperatura como MPI_DOUBLE. */
static MPI_Datatype criar_tipo_leitura(void) {
    int          blocos[2] = {1, 1};
    MPI_Aint     desloc[2] = {offsetof(Leitura, id), offsetof(Leitura, temp)};
    MPI_Datatype tipos[2]  = {MPI_INT, MPI_DOUBLE};
    MPI_Datatype tipo, redimensionado;

    MPI_Type_create_struct(2, blocos, desloc, tipos, &tipo);
    MPI_Type_create_resized(tipo, 0, sizeof(Leitura), &redimensionado);
    MPI_Type_free(&tipo);
    MPI_Type_commit(&redimensionado);
    return redimensionado;
}

static MPI_Datatype criar_tipo_evento(void) {
    int          blocos[3] = {1, 1, 1};
    MPI_Aint     desloc[3] = {offsetof(Evento, id), offsetof(Evento, temp),
                              offsetof(Evento, acao)};
    MPI_Datatype tipos[3]  = {MPI_INT, MPI_DOUBLE, MPI_INT};
    MPI_Datatype tipo, redimensionado;

    MPI_Type_create_struct(3, blocos, desloc, tipos, &tipo);
    MPI_Type_create_resized(tipo, 0, sizeof(Evento), &redimensionado);
    MPI_Type_free(&tipo);
    MPI_Type_commit(&redimensionado);
    return redimensionado;
}

static void dormir_ms(int ms) {
    struct timespec t = {ms / 1000, (long)(ms % 1000) * 1000000L};
    nanosleep(&t, NULL);
}

/* ---------------------------------------------------------------- Rank 0 */
static void sensor(int leituras, int intervalo_ms, MPI_Datatype tipo_leitura) {
    srand((unsigned)time(NULL));

    for (int i = 1; i <= leituras; i++) {
        Leitura l;
        l.id   = i;
        l.temp = TEMP_MIN + (TEMP_MAX - TEMP_MIN) * ((double)rand() / RAND_MAX);

        /* Síncrono: só segue para a próxima leitura depois que o
         * controlador casou o MPI_Recv com esta mensagem. */
        MPI_Ssend(&l, 1, tipo_leitura, CONTROLADOR, TAG_LEITURA, MPI_COMM_WORLD);
        printf("[Sensor]      leitura %3d: %6.2f °C enviada\n", l.id, l.temp);
        fflush(stdout);

        if (i < leituras)
            dormir_ms(intervalo_ms);
    }
}

/* ---------------------------------------------------------------- Rank 1 */
static void controlador(int leituras, MPI_Datatype tipo_leitura,
                        MPI_Datatype tipo_evento) {
    /* Memória pré-alocada para o MPI_Bsend: cabe um evento por leitura,
     * então o envio ao logger nunca precisa esperar espaço no buffer. */
    int tam_msg;
    MPI_Pack_size(1, tipo_evento, MPI_COMM_WORLD, &tam_msg);
    int   tam_buffer = leituras * (tam_msg + MPI_BSEND_OVERHEAD);
    void *buffer     = malloc(tam_buffer);
    if (!buffer) {
        fprintf(stderr, "Controlador: sem memória para o buffer do Bsend\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Buffer_attach(buffer, tam_buffer);

    for (int i = 0; i < leituras; i++) {
        Leitura l;
        MPI_Recv(&l, 1, tipo_leitura, SENSOR, TAG_LEITURA, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        Evento e;
        e.id   = l.id;
        e.temp = l.temp;
        e.acao = (l.temp > TEMP_LIMITE) ? 1 : 0;

        printf("[Controlador] leitura %3d: %6.2f °C -> válvula %s\n", e.id,
               e.temp, e.acao ? "ABERTA" : "fechada");
        fflush(stdout);

        /* Bufferizado: copia o evento para o buffer anexado e retorna
         * na hora, sem depender do logger estar pronto para receber. */
        MPI_Bsend(&e, 1, tipo_evento, LOGGER, TAG_EVENTO, MPI_COMM_WORLD);
    }

    /* O detach bloqueia até todas as mensagens bufferizadas saírem. */
    void *b;
    int   tam;
    MPI_Buffer_detach(&b, &tam);
    free(b);
}

/* ---------------------------------------------------------------- Rank 2 */
static void logger(int leituras, MPI_Datatype tipo_evento) {
    FILE *arq = fopen(ARQUIVO_LOG, "w");
    if (!arq)
        perror("Logger: não foi possível abrir " ARQUIVO_LOG " (só console)");
    else
        fprintf(arq, "id;temperatura_C;acao\n");

    int    aberturas = 0;
    double soma = 0.0, tmin = TEMP_MAX + 1, tmax = TEMP_MIN - 1;

    for (int i = 0; i < leituras; i++) {
        Evento e;
        MPI_Recv(&e, 1, tipo_evento, CONTROLADOR, TAG_EVENTO, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        printf("[Logger]      evento  %3d: %6.2f °C | acao = %d (%s)\n", e.id,
               e.temp, e.acao, e.acao ? "alívio acionado" : "normal");
        fflush(stdout);
        if (arq) {
            fprintf(arq, "%d;%.2f;%d\n", e.id, e.temp, e.acao);
            fflush(arq);
        }

        aberturas += e.acao;
        soma += e.temp;
        if (e.temp < tmin) tmin = e.temp;
        if (e.temp > tmax) tmax = e.temp;
    }

    printf("\n===== Relatório da caldeira =====\n");
    printf("Leituras registradas : %d\n", leituras);
    printf("Válvula aberta       : %d vezes (%.1f%%)\n", aberturas,
           100.0 * aberturas / leituras);
    printf("Válvula fechada      : %d vezes\n", leituras - aberturas);
    printf("Temperatura mínima   : %.2f °C\n", tmin);
    printf("Temperatura máxima   : %.2f °C\n", tmax);
    printf("Temperatura média    : %.2f °C\n", soma / leituras);
    if (arq) {
        printf("Log salvo em         : %s\n", ARQUIVO_LOG);
        fclose(arq);
    }
    fflush(stdout);
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, tamanho;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanho);

    if (tamanho != 3) {
        if (rank == 0)
            fprintf(stderr, "Use exatamente 3 processos: mpirun -np 3 %s\n",
                    argv[0]);
        MPI_Finalize();
        return 1;
    }

    /* Todos os ranks recebem os mesmos argumentos, então todos
     * concordam sobre quantas mensagens serão trocadas. */
    int leituras     = (argc > 1) ? atoi(argv[1]) : 100;
    int intervalo_ms = (argc > 2) ? atoi(argv[2]) : 1000;
    if (leituras < 1 || intervalo_ms < 0) {
        if (rank == 0)
            fprintf(stderr, "Uso: %s [leituras >= 1] [intervalo_ms >= 0]\n",
                    argv[0]);
        MPI_Finalize();
        return 1;
    }

    MPI_Datatype tipo_leitura = criar_tipo_leitura();
    MPI_Datatype tipo_evento  = criar_tipo_evento();

    switch (rank) {
        case SENSOR:      sensor(leituras, intervalo_ms, tipo_leitura);       break;
        case CONTROLADOR: controlador(leituras, tipo_leitura, tipo_evento);   break;
        case LOGGER:      logger(leituras, tipo_evento);                      break;
    }

    MPI_Type_free(&tipo_leitura);
    MPI_Type_free(&tipo_evento);
    MPI_Finalize();
    return 0;
}
