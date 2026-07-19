# OPZIP: Zip Paralelo com Escrita Ordenada

Sincronização produtor-consumidor com pthreads aplicada a compressão RLE.

**Autores:** Luiz Henrique Ribeiro Torres, Raissa Caroline Cavalcanti da Silva
**Disciplina:** Sistemas Operacionais I — Prof. Dr. Maxweel Silva Carmo
**Instituição:** Universidade Federal de Mato Grosso — Campus Araguaia — ICET
**Local/data:** Barra do Garças, MT — julho de 2026

## Sumário

- [Introdução](#introdução)
- [Fundamentação teórica](#fundamentação-teórica-e-relação-com-sistemas-operacionais)
- [Arquitetura do sistema](#arquitetura-do-sistema)
- [Detalhamento do código](#detalhamento-do-código)
- [Escolha de TAM_BLOCO e K](#escolha-de-tam_bloco-e-k)
- [Ambiente de testes](#ambiente-de-testes)
- [Speedup versus número de threads](#speedup-versus-número-de-threads)
- [Efeito do parâmetro K no desempenho](#efeito-do-parâmetro-k-no-desempenho)
- [O que acontece com K = 1](#o-que-acontece-com-k--1)
- [Mutex global versus mutex por posição](#mutex-global-versus-mutex-por-posição-do-buffer)
- [Considerações finais](#considerações-finais)
- [Referências](#referências)

## Introdução

O opzip é uma variante paralela de compressão run-length encoding (RLE) baseada em pthreads. O trabalho aprofunda o uso de threads POSIX — em particular mutexes e condition variables — na construção de um pipeline produtor-consumidor real, no qual blocos comprimidos são escritos no arquivo de saída assim que ficam prontos, respeitando a ordem original do arquivo, por meio de um buffer circular limitado compartilhado entre threads produtoras e uma única thread consumidora (escritora).

Diferentemente do modelo fork-join clássico (em que cada thread comprime seu pedaço isoladamente e os resultados são concatenados ao final), o opzip mantém sincronização contínua durante toda a execução, o que exige cuidado adicional de projeto para equilibrar paralelismo e ordem de escrita.

## Fundamentação teórica e relação com Sistemas Operacionais

O opzip é um exercício aplicado de três conceitos centrais da disciplina: threads como unidade de execução concorrente, exclusão mútua sobre regiões críticas compartilhadas, e sincronização condicional entre threads que dependem do progresso umas das outras.

### Threads como abstração de execução concorrente

Uma thread é uma unidade de execução que compartilha o espaço de endereçamento de um processo, mas mantém seu próprio contexto (pilha, registradores, contador de programa). O sistema operacional escalona essas threads entre os núcleos disponíveis, alternando o contexto de execução de forma transparente ao programador. No opzip, essa abstração é explorada criando N threads produtoras que competem pelo mesmo conjunto de blocos de trabalho, e uma thread consumidora dedicada. Como as threads compartilham memória (buffer de entrada, índice de próximo bloco e buffer circular de saída), qualquer acesso concorrente exige sincronização explícita, sob risco de race conditions.

### Exclusão mútua com mutexes

Dois mutexes distintos protegem duas regiões críticas independentes:

- `mutex_bloco`: protege apenas a leitura e o incremento do contador `proximo_bloco` (operação curtíssima).
- `mutex_buffer`: protege o acesso às K posições do buffer circular compartilhado entre produtoras e consumidora.

Separar essas regiões em mutexes distintos evita que a distribuição de blocos (rápida) fique bloqueada esperando por operações de buffer (potencialmente mais lentas), reduzindo contenção desnecessária entre partes não relacionadas do algoritmo.

### O problema do produtor-consumidor e o buffer limitado

O opzip é uma instanciação direta do clássico bounded buffer problem (capítulo de Condition Variables do OSTEP). A complicação adicional em relação à versão didática é que os blocos comprimidos precisam ser escritos na ordem original do arquivo, e não na ordem em que ficam prontos. Essa restrição transforma o buffer circular simples em uma janela deslizante sobre uma sequência ordenada de blocos (detalhada na seção 4.3).

### Condition variables e a eliminação de busy-waiting

O opzip evita polling utilizando `pthread_cond_wait` exclusivamente: uma thread que precisa esperar por uma condição (espaço livre no buffer, ou dados prontos) libera atomicamente o mutex e suspende sua execução, sendo devolvida à fila de prontos pelo escalonador apenas quando outra thread sinaliza a condition variable correspondente. Isso foi verificado experimentalmente: mesmo em cenários de forte serialização (K = 1), o tempo de CPU consumido pelas threads bloqueadas permaneceu próximo de zero.

### Race conditions e deadlock

- **Race condition:** verificada ausente por meio do ThreadSanitizer (`-fsanitize=thread`).
- **Deadlock:** um bug real foi identificado e corrigido durante o desenvolvimento (detalhado na seção 4.3), no qual uma condição de espera insuficientemente restritiva permitia que um bloco de índice alto ocupasse prematuramente uma posição do buffer, travando a consumidora à espera de um bloco de índice mais baixo que nunca conseguiria ser escrito naquela posição.

## Arquitetura do sistema

A thread principal lê o arquivo de entrada inteiro para memória em uma única leitura sequencial e calcula `num_blocos = ceil(tamanho_arquivo / TAM_BLOCO)`. Um índice compartilhado, `proximo_bloco`, protegido por mutex, distribui os blocos dinamicamente entre as threads produtoras: cada thread retira o próximo índice disponível assim que termina o bloco anterior, balanceando automaticamente a carga entre threads mais rápidas e mais lentas, sem atribuição estática de blocos por thread.

A comunicação entre produtoras e consumidora é feita por um buffer circular de K posições, protegido por um único mutex global (`mutex_buffer`), com duas condition variables:

- `cond_slot_livre`: produtoras aguardam espaço.
- `cond_slot_pronto`: consumidora aguarda dados.

Toda sincronização utiliza `pthread_cond_wait`, sem polling.

## Detalhamento do código

### Estrutura de dados do buffer circular

```c
typedef struct {
    int indice_bloco;
    char *dados_comprimidos;
    size_t tamanho_comprimido;
    int pronto;
} Slot;
```

O campo `indice_bloco` é necessário para a consumidora validar se o dado presente é realmente o próximo a ser escrito. `pronto` indica se a posição contém dados válidos ainda não consumidos.

### Compressão run-length encoding

A função `comprimir_bloco` percorre o bloco contando repetições consecutivas do mesmo caractere e grava cada run como um registro de 5 bytes (4 bytes de contagem em `int32_t` seguidos de 1 byte de caractere), usando `memcpy` em vez de escrever a struct diretamente, evitando padding inserido pelo compilador:

```c
static char *comprimir_bloco(const char *dados, size_t tamanho,
                              size_t *tamanho_saida) {
    size_t capacidade = (tamanho == 0) ? 1 : tamanho * 5;
    char *saida_buf = malloc(capacidade);
    size_t pos = 0, i = 0;

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
```

O buffer de saída é alocado com o tamanho de pior caso (`tamanho * 5`), correspondente a um arquivo sem nenhum caractere repetido, em que cada byte de entrada vira uma run individual de 5 bytes de saída.

### Thread produtora e a janela deslizante

Cada thread produtora retira o próximo índice de bloco disponível, comprime esse bloco fora de qualquer região crítica (para não bloquear outras threads durante o trabalho pesado de CPU) e disputa o mutex do buffer apenas para inserir o resultado já pronto:

```c
pthread_mutex_lock(&mutex_buffer);
while (idx >= proximo_a_escrever + K) {
    pthread_cond_wait(&cond_slot_livre, &mutex_buffer);
}
buffer[posicao].indice_bloco = idx;
buffer[posicao].dados_comprimidos = comp;
buffer[posicao].tamanho_comprimido = tamanho_comp;
buffer[posicao].pronto = 1;
pthread_cond_broadcast(&cond_slot_pronto);
pthread_mutex_unlock(&mutex_buffer);
```

A condição `idx >= proximo_a_escrever + K` é o ponto mais delicado do projeto. Como os blocos são distribuídos dinamicamente, não há garantia de que terminem de ser comprimidos na mesma ordem em que foram distribuídos: uma thread pode terminar um bloco de índice alto (por exemplo, `idx = 10`, com `K = 8`, ocupando a posição `10 mod 8 = 2`) antes de outra thread terminar o bloco de índice mais baixo que ocupa a mesma posição (`idx = 2`).

Se a condição de espera checasse apenas `pronto`, a posição 2 apareceria como livre e o bloco 10 seria inserido prematuramente. Quando o bloco 2 terminasse de ser comprimido, sua thread encontraria a posição ocupada pelo bloco 10 e ficaria bloqueada esperando ela ser liberada, enquanto a consumidora espera exatamente pelo bloco 2 naquela posição — resultando em deadlock permanente.

A condição baseada na janela deslizante `[proximo_a_escrever, proximo_a_escrever + K)` impede que qualquer bloco além dessa janela seja inserido, garantindo que uma posição só seja reutilizada depois que o bloco anterior que a ocupava já tenha sido efetivamente escrito pela consumidora. Esse bug foi encontrado durante os testes de correção, ao observar que arquivos com mais de aproximadamente 2K blocos travavam indefinidamente com a implementação ingênua, e foi corrigido antes da entrega final.

### Thread consumidora

A thread consumidora mantém um contador `proximo_a_escrever` e, para cada bloco, espera até que a posição correspondente contenha exatamente o índice esperado:

```c
pthread_mutex_lock(&mutex_buffer);
while (!(buffer[posicao].pronto &&
         buffer[posicao].indice_bloco == proximo_a_escrever)) {
    pthread_cond_wait(&cond_slot_pronto, &mutex_buffer);
}
char *dados = buffer[posicao].dados_comprimidos;
size_t tamanho = buffer[posicao].tamanho_comprimido;
pthread_mutex_unlock(&mutex_buffer);

fwrite(dados, 1, tamanho, saida);
free(dados);

pthread_mutex_lock(&mutex_buffer);
buffer[posicao].pronto = 0;
proximo_a_escrever++;
pthread_cond_broadcast(&cond_slot_livre);
pthread_mutex_unlock(&mutex_buffer);
```

A escrita em disco (`fwrite`) é feita fora da região crítica, liberando o mutex antes de uma operação de I/O potencialmente lenta, de modo que threads produtoras podem continuar inserindo blocos no buffer enquanto a consumidora escreve o bloco anterior no arquivo de saída.

### Distribuição dinâmica de blocos e encerramento

```c
pthread_mutex_lock(&mutex_bloco);
if (proximo_bloco >= num_blocos) {
    pthread_mutex_unlock(&mutex_bloco);
    break;
}
int idx = proximo_bloco;
proximo_bloco++;
pthread_mutex_unlock(&mutex_bloco);
```

Quando `proximo_bloco` atinge `num_blocos`, cada thread produtora encerra seu laço naturalmente e retorna. A thread principal executa `pthread_join` em todas as produtoras e, por fim, na consumidora (que só termina quando `proximo_a_escrever` alcança `num_blocos`), garantindo encerramento limpo sem loops infinitos ou threads órfãs. Toda memória alocada dinamicamente é liberada antes do programa terminar.

## Escolha de TAM_BLOCO e K

**TAM_BLOCO = 1 MB**, independente do número de threads. Blocos muito pequenos aumentam o overhead de sincronização (mais operações de lock e unlock por byte processado) e degradam a taxa de compressão nas bordas dos blocos; blocos muito grandes reduzem o paralelismo disponível e aumentam a latência até a primeira escrita no arquivo de saída. O valor de 1 MB equilibra os dois efeitos para arquivos de dezenas de megabytes ou mais.

**K = 2 × número de threads.** Um K pequeno demais força as produtoras a aguardar com frequência por espaço no buffer, serializando o pipeline; um K grande demais consome mais memória sem benefício adicional, já que o gargalo real é a taxa de escrita da consumidora, única e sequencial. Um K equivalente ao dobro do número de threads dá folga suficiente para que threads mais rápidas não fiquem bloqueadas esperando a consumidora, sem desperdiçar memória.

## Ambiente de testes

Os experimentos de desempenho foram executados em um MacBook Pro com processador multi-núcleo, utilizando um arquivo de teste de aproximadamente 150 MB, com conteúdo parcialmente repetitivo gerado sinteticamente. Cada medição corresponde à mediana de três execuções. O script `benchmark.py`, incluído na entrega, automatiza a coleta dessas medidas e a geração dos gráficos.

A correção funcional foi validada separadamente com arquivos vazios, menores que um bloco, múltiplos exatos do tamanho de bloco, altamente repetitivos e com dados aleatórios, comparando byte a byte a saída do descompressor de referência (`ounzip`) com o arquivo original, além de execuções com ThreadSanitizer sem nenhum aviso de data race.

## Speedup versus número de threads

Speedup medido (T1/Tn) em função do número de threads produtoras, comparado ao speedup ideal linear:

| Threads | Speedup medido |
|---|---|
| 1 | 1,00x |
| 2 | 1,84x |
| 4 | 2,78x |
| 8 | 3,29x |
| 16 | 3,27x |

O speedup cresce de forma consistente até 8 threads, aproximando-se de um comportamento quase linear nessa faixa. A partir de 8 threads o ganho estagna, indicando saturação do paralelismo disponível na máquina de teste. Dois fatores explicam esse platô:

1. O número de threads passou a exceder o número de núcleos físicos úteis para esta carga, de modo que threads adicionais competem por CPU em vez de acrescentar paralelismo real.
2. A thread consumidora é única e escreve sequencialmente no arquivo de saída, tornando-se o gargalo estrutural do pipeline — acima de certo número de produtoras, a taxa de escrita da consumidora passa a limitar o desempenho total, independentemente de quantas threads comprimam blocos em paralelo.

Esse comportamento ilustra a Lei de Amdahl aplicada ao pipeline: a fração sequencial do algoritmo (a escrita ordenada) limita o speedup máximo alcançável, independentemente de quantas threads produtoras sejam adicionadas.

## Efeito do parâmetro K no desempenho

Tempo de execução em função de K, com o número de threads fixo em 4:

| K | Tempo (s) |
|---|---|
| 1 | 0,195 |
| 2 | 0,196 |
| 4 | 0,213 |
| 8 | 0,196 |
| 16 | 0,197 |
| 32 | 0,194 |
| 64 | 0,178 |
| 128 | 0,186 |

Diferentemente do esperado teoricamente, os tempos medidos variam pouco entre os diferentes valores de K (entre 0,178 s e 0,213 s), sem uma tendência monótona clara. Isso sugere que, para este arquivo de teste e TAM_BLOCO de 1 MB com apenas 4 threads produtoras, a consumidora consegue escrever os blocos praticamente no ritmo em que ficam prontos, de modo que o buffer raramente chega a ficar cheio mesmo com K pequeno. A variável dominante nas pequenas diferenças observadas passa a ser o ruído natural do sistema operacional (agendamento de threads, cache, outras cargas da máquina), e não o próprio tamanho do buffer.

O efeito de K é mais pronunciado quando há descompasso real entre a velocidade de compressão (várias threads) e a velocidade de escrita (uma thread só), cenário explorado de forma mais direta a seguir.

## O que acontece com K = 1?

Com K = 1, o buffer circular possui uma única posição, o que significa que uma produtora só pode escrever o bloco `i` depois que a consumidora escreveu o bloco `i - 1` no arquivo de saída. Isso elimina, na prática, o paralelismo do estágio de bufferização: mesmo com várias threads comprimindo blocos diferentes simultaneamente, apenas um bloco por vez pode estar em trânsito no buffer, e as demais threads produtoras ficam bloqueadas em `pthread_cond_wait` aguardando a liberação da única posição disponível.

No experimento realizado (t = 4 threads), K = 1 resultou em 0,195 s, valor muito próximo da maioria dos demais K testados, sem a penalidade acentuada que a teoria sugeriria. A explicação mais provável é que, com blocos de 1 MB, o tempo gasto comprimindo cada bloco é comparável ao tempo gasto escrevendo-o no disco, de modo que a consumidora praticamente não acumula atraso mesmo sendo obrigada a liberar a posição única antes de cada nova escrita.

O efeito de K = 1 tende a ficar mais evidente com blocos menores e mais threads produtoras, cenário em que a velocidade agregada de compressão supera com folga a velocidade de escrita sequencial. Nessas condições, K = 1 forçaria o sistema a se comportar como um pipeline essencialmente serial, com apenas a compressão do próximo bloco sobrepondo-se à escrita do bloco atual.

## Mutex global versus mutex por posição do buffer

A solução entregue (`opzip.c`) usa um único mutex global protegendo todas as K posições do buffer: qualquer operação em qualquer posição, mesmo quando threads diferentes mexem em posições diferentes, precisa adquirir esse mesmo lock, serializando o acesso ao buffer inteiro.

Como alternativa, foi implementada uma variante experimental (`opzip_por_posicao.c`, incluída apenas para este experimento, e não como a solução principal) com um mutex e uma condition variable dedicados a cada posição do buffer, além de um mutex separado apenas para a variável de progresso (`proximo_a_escrever`). Nessa variante, threads que escrevem em posições diferentes do buffer não disputam o mesmo lock entre si.

Para medir o impacto da contenção, ambas as versões foram executadas com blocos pequenos (4 KB), maximizando a taxa de operações de lock por segundo:

| Threads | Mutex global (s) | Mutex por posição (s) |
|---|---|---|
| 2 | 0,097 | 0,103 |
| 4 | 0,064 | 0,062 |
| 8 | 0,069 | 0,056 |
| 16 | 0,205 | 0,147 |
| 32 | 0,178 | 0,178 |

Com 8 threads, o mutex global levou 0,069 s contra 0,056 s da versão por posição (cerca de 19% mais rápida); com 16 threads, a diferença é ainda mais acentuada, 0,205 s contra 0,147 s (cerca de 28% mais rápida). Com poucas threads (2 e 4) as duas versões apresentam desempenho semelhante, e com 32 threads ambas convergem para um tempo próximo, provavelmente porque, nesse ponto, o gargalo deixa de ser a disputa pelo lock do buffer e passa a ser a saturação de núcleos disponíveis ou a própria escrita sequencial da consumidora.

Esse padrão é consistente com a teoria: com o mutex global, toda operação no buffer fica serializada mesmo quando threads distintas mexem em posições distintas, criando um ponto de contenção artificial que se intensifica à medida que mais threads disputam o mesmo lock. A versão por posição elimina essa disputa desnecessária, permitindo que threads em posições diferentes progridam verdadeiramente em paralelo. O custo dessa abordagem é a complexidade adicional de manter K + 1 pares de mutex e condition variable, em vez de apenas um, e a necessidade de manter dois níveis de sincronização coerentes entre si: a janela deslizante global e o estado individual de cada slot.

## Considerações finais

A implementação do opzip demonstrou, na prática, os principais conceitos trabalhados na disciplina de Sistemas Operacionais: uso de mutexes e condition variables para sincronização contínua entre múltiplas threads, distribuição dinâmica de trabalho para balanceamento automático de carga, e o trade-off entre paralelismo e preservação de ordem na saída.

O processo de desenvolvimento também evidenciou os riscos clássicos da programação concorrente: um bug real de deadlock foi encontrado e corrigido durante os testes, decorrente de uma condição de sincronização aparentemente correta à primeira vista, mas insuficiente diante da distribuição dinâmica de blocos.

Os experimentos de desempenho mostraram speedup próximo do linear até o número de threads compatível com os núcleos disponíveis na máquina de teste, com estagnação a partir daí devido ao gargalo estrutural da escrita sequencial única. A comparação entre mutex global e mutexes por posição do buffer confirmou que a granularidade da sincronização passa a importar à medida que o número de threads cresce, ainda que às custas de maior complexidade de implementação.

Esses resultados reforçam que, em sistemas concorrentes, corretude e desempenho são preocupações interdependentes: uma sincronização excessivamente simples pode ser incorreta, enquanto uma sincronização excessivamente granular pode não compensar sua complexidade adicional dependendo da carga de trabalho.

## Referências

ARPACI-DUSSEAU, Remzi H.; ARPACI-DUSSEAU, Andrea C. **Operating Systems: Three Easy Pieces**. Capítulos: Introduction to Threads; Threads API; Locks; Using Locks; Condition Variables. Arpaci-Dusseau Books, 2018.
