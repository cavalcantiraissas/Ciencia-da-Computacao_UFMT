/*
 * Alerta climático com tipo derivado MPI (MPI_Type_create_struct).
 *
 *   [Rank 0: Estação] --(MPI_Send: 1 x MPI_Leitura_Clima)--> [Rank 1: Central]
 *   [Rank 1: Central] --(MPI_Send: 1 x MPI_Leitura_Clima)--> [Rank 0: Estação]
 *
 * - Estação: monta cada leitura (id, temperatura, umidade) com alerta_ativo
 *   ainda indefinido ('?') e envia a struct inteira como 1 elemento.
 * - Central: preenche alerta_ativo com 'S' se alguma condição crítica for
 *   atendida, ou 'N' caso contrário, e devolve a struct à estação:
 *     - Calor extremo e baixa umidade: temperatura >= 35.0 °C e umidade <= 20.0 %
 *     - Tempestade / ar saturado:      temperatura >= 30.0 °C e umidade >= 85.0 %
 *     - Geada / frio intenso:          temperatura <= 5.0 °C
 * - A estação imprime o resultado e, nos casos de teste fixos (que cobrem as
 *   fronteiras de cada condição), confere com o valor esperado.
 *
 * Etapas do tipo derivado (criar_tipo_leitura):
 *   1. MPI_Get_address: deslocamento de cada membro a partir do início da
 *      struct, respeitando o padding que o compilador insere (o char final e o
 *      int inicial não ficam colados aos doubles).
 *   2. MPI_Type_create_struct: combina MPI_INT, MPI_DOUBLE e MPI_CHAR.
 *   3. MPI_Type_commit: registra o tipo antes do envio.
 *   4. MPI_Send/MPI_Recv com count = 1 e, ao final, MPI_Type_free.
 *
 * Compilar: mpicc -O2 estacao_clima_mpi.c -o estacao_clima_mpi
 * Executar: mpirun -np 2 ./estacao_clima_mpi [leituras_aleatorias]
 *           (padrão: 5 leituras aleatórias após os casos fixos)
 */
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ESTACAO 0
#define CENTRAL 1

#define TAG_LEITURA  10
#define TAG_RESPOSTA 20

typedef struct {
    int    id_estacao;
    double temperatura;   /* °C */
    double umidade;       /* % */
    char   alerta_ativo;  /* 'S' ou 'N' ('?' antes da avaliação) */
} LeituraClima;

/* Casos fixos com o alerta esperado, incluindo os valores de fronteira. */
static const struct {
    double temperatura, umidade;
    char   esperado;
    const char *descricao;
} CASOS[] = {
    {38.0, 15.0, 'S', "calor extremo e baixa umidade"},
    {35.0, 20.0, 'S', "calor: fronteira exata (35.0 / 20.0)"},
    {35.0, 20.1, 'N', "calor, mas umidade acima de 20 %"},
    {34.9, 10.0, 'N', "seco, mas abaixo de 35 °C"},
    {32.0, 90.0, 'S', "tempestade / ar saturado"},
    {30.0, 85.0, 'S', "tempestade: fronteira exata (30.0 / 85.0)"},
    {29.9, 95.0, 'N', "saturado, mas abaixo de 30 °C"},
    { 2.0, 70.0, 'S', "geada / frio intenso"},
    { 5.0, 40.0, 'S', "frio: fronteira exata (5.0)"},
    { 5.1, 40.0, 'N', "frio, mas acima de 5 °C"},
    {24.0, 60.0, 'N', "condição amena"},
};
#define N_CASOS ((int)(sizeof CASOS / sizeof CASOS[0]))

/* ------------------------------------------------- Tipo derivado MPI */
static MPI_Datatype criar_tipo_leitura(void) {
    LeituraClima modelo;
    MPI_Aint     base, enderecos[4];

    /* 1. Cálculo de deslocamentos */
    MPI_Get_address(&modelo,              &base);
    MPI_Get_address(&modelo.id_estacao,   &enderecos[0]);
    MPI_Get_address(&modelo.temperatura,  &enderecos[1]);
    MPI_Get_address(&modelo.umidade,      &enderecos[2]);
    MPI_Get_address(&modelo.alerta_ativo, &enderecos[3]);

    MPI_Aint desloc[4];
    for (int i = 0; i < 4; i++)
        desloc[i] = MPI_Aint_diff(enderecos[i], base);

    /* 2. Criação do tipo */
    int          blocos[4] = {1, 1, 1, 1};
    MPI_Datatype tipos[4]  = {MPI_INT, MPI_DOUBLE, MPI_DOUBLE, MPI_CHAR};
    MPI_Datatype tipo, tipo_leitura;
    MPI_Type_create_struct(4, blocos, desloc, tipos, &tipo);

    /* A extensão calculada pelo MPI termina no char; ajusta para
     * sizeof(LeituraClima) para incluir o padding do final da struct. */
    MPI_Type_create_resized(tipo, 0, sizeof(LeituraClima), &tipo_leitura);
    MPI_Type_free(&tipo);

    /* 3. Compilação do tipo */
    MPI_Type_commit(&tipo_leitura);
    return tipo_leitura;
}

/* ------------------------------------------------- Regra de alerta */
static char avaliar_alerta(double temperatura, double umidade) {
    int calor_seco = temperatura >= 35.0 && umidade <= 20.0;
    int tempestade = temperatura >= 30.0 && umidade >= 85.0;
    int geada      = temperatura <= 5.0;
    return (calor_seco || tempestade || geada) ? 'S' : 'N';
}

/* ------------------------------------------------------------ Rank 0 */
static void estacao(int total, MPI_Datatype tipo_leitura) {
    srand((unsigned)time(NULL));
    int erros = 0, alertas = 0;

    printf("  id |  temp (°C) | umid (%%) | alerta | conferência\n");
    printf("-----+------------+----------+--------+---------------------------\n");

    for (int i = 0; i < total; i++) {
        LeituraClima l;
        l.id_estacao   = i + 1;
        l.alerta_ativo = '?';
        if (i < N_CASOS) {
            l.temperatura = CASOS[i].temperatura;
            l.umidade     = CASOS[i].umidade;
        } else {
            l.temperatura = -5.0 + 47.0 * ((double)rand() / RAND_MAX); /* -5 a 42 °C */
            l.umidade     =  5.0 + 95.0 * ((double)rand() / RAND_MAX); /*  5 a 100 % */
        }

        /* 4. Comunicação: a struct inteira como 1 elemento do tipo novo */
        MPI_Send(&l, 1, tipo_leitura, CENTRAL, TAG_LEITURA, MPI_COMM_WORLD);
        MPI_Recv(&l, 1, tipo_leitura, CENTRAL, TAG_RESPOSTA, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);

        alertas += (l.alerta_ativo == 'S');
        printf("%4d | %10.1f | %8.1f |   %c    | ", l.id_estacao, l.temperatura,
               l.umidade, l.alerta_ativo);
        if (i < N_CASOS) {
            int ok = (l.alerta_ativo == CASOS[i].esperado);
            erros += !ok;
            printf("%s %s\n", ok ? "ok  " : "ERRO", CASOS[i].descricao);
        } else {
            printf("(aleatória)\n");
        }
    }

    printf("\nLeituras: %d | alertas 'S': %d | casos fixos com erro: %d\n",
           total, alertas, erros);
}

/* ------------------------------------------------------------ Rank 1 */
static void central(int total, MPI_Datatype tipo_leitura) {
    for (int i = 0; i < total; i++) {
        LeituraClima l;
        MPI_Recv(&l, 1, tipo_leitura, ESTACAO, TAG_LEITURA, MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        l.alerta_ativo = avaliar_alerta(l.temperatura, l.umidade);
        MPI_Send(&l, 1, tipo_leitura, ESTACAO, TAG_RESPOSTA, MPI_COMM_WORLD);
    }
}

int main(int argc, char **argv) {
    MPI_Init(&argc, &argv);

    int rank, tamanho;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tamanho);

    if (tamanho != 2) {
        if (rank == 0)
            fprintf(stderr, "Use exatamente 2 processos: mpirun -np 2 %s\n", argv[0]);
        MPI_Finalize();
        return 1;
    }

    int aleatorias = (argc > 1) ? atoi(argv[1]) : 5;
    if (aleatorias < 0) {
        if (rank == 0)
            fprintf(stderr, "Uso: %s [leituras_aleatorias >= 0]\n", argv[0]);
        MPI_Finalize();
        return 1;
    }
    int total = N_CASOS + aleatorias;

    MPI_Datatype tipo_leitura = criar_tipo_leitura();

    if (rank == ESTACAO) estacao(total, tipo_leitura);
    else                 central(total, tipo_leitura);

    /* 4. Liberação */
    MPI_Type_free(&tipo_leitura);
    MPI_Finalize();
    return 0;
}
