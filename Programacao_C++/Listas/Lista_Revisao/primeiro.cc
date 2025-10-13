/*
Universidade Federal de Mato Grosso
Programação II = Lista de Revisão
Aluna: Raissa Cavalcanti

-> Exercício Um -> Round Robin

*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

struct Servidor {
    string nome;
    int peso;
    int solicitacoes_atendidas;
};

void roundRobin(vector<Servidor>& servidores, int num_solicitacoes) {
    int num_servidores = servidores.size();
    int indice_servidor = 0;
    
    for (int i = 0; i < num_solicitacoes; ++i) {
        servidores[indice_servidor].solicitacoes_atendidas++;
        indice_servidor = (indice_servidor + 1) % num_servidores;
    }
}

void weightedRoundRobin(vector<Servidor>& servidores, int num_solicitacoes) {
    int num_servidores = servidores.size();
    int total_pesos = 0;
    
    for (const Servidor& servidor : servidores) {
        total_pesos += servidor.peso;
    }
    
    int indice_servidor = 0;
    
    for (int i = 0; i < num_solicitacoes; ++i) {
        servidores[indice_servidor].solicitacoes_atendidas++;
        
        indice_servidor = (indice_servidor + 1) % num_servidores;
        while ((i + 1) % (num_solicitacoes / total_pesos) == 0 && servidores[indice_servidor].peso > 1) {
            indice_servidor = (indice_servidor + 1) % num_servidores;
            servidores[indice_servidor].peso--;
        }
    }
}

void randomStrategy(vector<Servidor>& servidores, int num_solicitacoes) {
    int num_servidores = servidores.size();
    
    srand(time(nullptr));
    
    for (int i = 0; i < num_solicitacoes; ++i) {
        int indice_servidor = rand() % num_servidores;
        servidores[indice_servidor].solicitacoes_atendidas++;
    }
}

int main() {
    int num_servidores;
    cout << "Digite o numero de servidores: ";
    cin >> num_servidores;
    
    vector<Servidor> servidores(num_servidores);
    
    for (int i = 0; i < num_servidores; ++i) {
        cout << "Digite o nome do servidor " << i + 1 << ": ";
        cin >> servidores[i].nome;
        cout << "Digite o peso do servidor " << i + 1 << ": ";
        cin >> servidores[i].peso;
        servidores[i].solicitacoes_atendidas = 0;
    }
    
    int num_solicitacoes;
    cout << "Digite o numero de requisicoes a serem injetadas: ";
    cin >> num_solicitacoes;
    
    roundRobin(servidores, num_solicitacoes);
    
    cout << "\nResultados do Round Robin:\n";
    for (const Servidor& servidor : servidores) {
        cout << "Servidor " << servidor.nome << ": " << servidor.solicitacoes_atendidas << " solicitacoes\n";
    }
    
    for (int i = 0; i < num_servidores; ++i) {
        servidores[i].solicitacoes_atendidas = 0;
    }
    
    weightedRoundRobin(servidores, num_solicitacoes);
    
    cout << "\nResultados do Weighted Round Robin:\n";
    for (const Servidor& servidor : servidores) {
        cout << "Servidor " << servidor.nome << ": " << servidor.solicitacoes_atendidas << " solicitacoes\n";
    }
    
    for (int i = 0; i < num_servidores; ++i) {
        servidores[i].solicitacoes_atendidas = 0;
    }
    
    randomStrategy(servidores, num_solicitacoes);
    
    cout << "\nResultados do Random:\n";
    for (const Servidor& servidor : servidores) {
        cout << "Servidor " << servidor.nome << ": " << servidor.solicitacoes_atendidas << " solicitacoes\n";
    }
    
    return 0;
}
