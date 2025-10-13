--- Pseudo código do método de ordenação Quick Sort
Funcao quickSort(vetor[], inteiro: inicio, inteiro: fim)
	inteiro: meio, i, j, pivo, aux
    
    //há diferentes maneiras de definir o pivo:
    //Ex1:
    pivo <- vetor[dir]
    //Ex2:
    meio <- ( (fim - inicio) / 2 ) + inicio;
    pivo <- vetor[meio]
    // Mas a estratégia de definição do pivo, adotada acima, vai impactar no procedimento de troca
    
	i <- inicio
	j <- fim

    //Faz as trocas entre os valores menores e maiores posicionados em lados errados (em relação ao pivo)
    enquanto ( i < j )
		enquanto ( vetor[i] < pivo )
			i <- i+1
	
		enquanto ( vetor[j] > pivo ) 
			j <- j-1

		//Verifica se os dois não se encontraram para fazer troca
		se ( i < j ) então
			aux <- vetor[i]
			vetor[i] <- vetor[j]
			vetor[j] <- aux
		
	
	//Verifica e faz a chamada recursiva
	se ( j > inicio )
		quickSort(vetor, inicio, j);
	
	se ( i < fim )
		quickSort(vetor, i, fim);
