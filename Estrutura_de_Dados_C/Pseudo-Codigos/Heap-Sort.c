Funcao principal ( ) {
     vetor = {5,2,7,6,10}   /*Cria um vetor para teste*/
     heapSort(vetor, 5)
}

Funcao heapSort (vetor, qtdElementos) {
     tipoHeap heap
     heap = insere elementos do vetor um a um no heap
     enquanto (heap != vazio) faça
          vetorOrdenado[] = removeRaizHeap(heap)
     fim enquanto
     retorna vetorOrdenado
} 
