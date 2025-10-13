/*
Universidade Federal de Mato Grosso
Programacao II || Professor Sandino
Discente: Raissa Cavalcanti || 16 de Maio de 2024

Atividade: Criar três funções: Reset, incrementa e dobra. Não retornam nada e atuam sobre uma única variável
Simular um agoritmo TCP (aumentando grativamente a retornando ao valor inicial quando ocorre uma perda)
Crie um la ̧co de repeticao no programa onde cada iteracao simulara um segundo de uma transmissao TCP onde a janela comeca em 1, 
incrementa de 1 em 1 nas cinco proximas interacoes e, se nao houve perda, passa a dobrar o tamanho da janela ate atingir um valor maximo (nao deve ultrapassar 100). 
O laco deve estar limitado a 100 repeticoes.
*/
#include <iostream>

void reset(int &janela) {
    janela = 1; 
}

void incrementa(int &janela) {
        janela++;
}

void dobra(int &janela) {
    if(janela *2>100){
            janela = janela; 
        } else{
             janela *= 2; 
        }
}

int main() {
    int janela = 1; 

    std::string output; 

    int instantes_de_perda[] = {10, 23, 77};
    int perda_index = 0;

    for (int i = 1; i <= 100; ++i) {
        output.clear();

        output += std::to_string(i) + " | ";

   
        if (perda_index < 3 && i == instantes_de_perda[perda_index]) {
            output += "perda";
            reset(janela); 
            perda_index++; 
        } else {
            
            output += std::to_string(janela);
            if (janela < 6) {
                incrementa(janela);
            } else {
                dobra(janela);
            }
        }

        std::cout << output << std::endl;
    }

    return 0;
}
