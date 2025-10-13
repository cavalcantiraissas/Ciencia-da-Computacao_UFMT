/*
Universidade Federal de Mato Grosso
Disciplina : Programacao II || Lista 04
Aluna: Raissa C. Cavalcanti

Exercício DOIS
*/

#include <iostream>
#include <vector>
#include <memory>

using namespace std;


class Empregado {
public:
    virtual double getSalarioMensal() const = 0; 
    virtual void imprimirDetalhes() const = 0;   
    virtual ~Empregado() {}                     
};

class EmpregadoAssalariado : public Empregado {
private:
    double salarioMensal;
public:
    EmpregadoAssalariado(double salario) : salarioMensal(salario) {}

    double getSalarioMensal() const override {
        return salarioMensal;
    }

    void imprimirDetalhes() const override {
        cout << "Empregado Assalariado, Salário Mensal: " << salarioMensal << endl;
    }
};

class EmpregadoHorista : public Empregado {
private:
    double horasTrabalhadas;
    double taxaPorHora;
public:
    EmpregadoHorista(double horas, double taxa)
        : horasTrabalhadas(horas), taxaPorHora(taxa) {}

    double getSalarioMensal() const override {
        return horasTrabalhadas * taxaPorHora;
    }

    void imprimirDetalhes() const override {
        cout << "Empregado Horista, Horas Trabalhadas: " << horasTrabalhadas
             << ", Taxa por Hora: " << taxaPorHora << endl;
    }
};

int main() {
    vector<shared_ptr<Empregado>> empregados;
    empregados.push_back(make_shared<EmpregadoAssalariado>(3000.00));
    empregados.push_back(make_shared<EmpregadoHorista>(160, 25.00));

    for (const auto& empregado : empregados) {
        empregado->imprimirDetalhes();
        cout << "Salário Mensal: " << empregado->getSalarioMensal() << endl;
    }

    return 0;
}
