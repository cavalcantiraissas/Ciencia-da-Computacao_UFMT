--- Pseudo código do QUICK SORT ---
Função quicksort(vetor[], inteiro: esq, inteiro: dir)
inicio
    inteiro: posicaoPivo
    se (esq < dir) então
        posicaoPivo <- particiona(vetor[], esq, dir)
        quicksort(vetor[], esq, (posicaoPivo – 1) )
        quicksort(vetor[], (posicaoPivo + 1), dir)
    fim se
fim

Funcao particiona(vetor[], esq, dir)
inicio
    inteiro: pivo, i, j
    //há diferentes maneiras de definir o pivo:
    //Ex1:
    pivo <- vetor[dir]
    //Ex2:
    pivo <- vetor[(dir-esq)/2]
    // Mas a estratégia de definição do pivo, adotada acima, vai impactar no procedimento de troca
    // A implementação a seguir considera o Ex1
    
    i <- esq
    //Este laço coloca todos os valores menores que pivo à esquerda e maiores à direita
    para j <- esq, enquanto j < (dir-1), passo 1 faça
        se (vetor[j] <= pivo) então
            troque vetor[i] com vetor[j]
            i <- i + 1
        fim se
    fim para
    troque vetor[i] com vetor[dir] //Coloca o pivo no local correto
    retorne i
fim
