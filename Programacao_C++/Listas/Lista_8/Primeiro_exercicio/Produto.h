#ifndef PRODUTO_H
#define PRODUTO_H

#include <string>

namespace Cadastro {

class Produto {
private:
    int codigo;
    std::string nome;
    double preco;
    int quantidade;

public:
    Produto(int codigo, const std::string &nome, double preco, int quantidade);
    int getCodigo() const;
    std::string getNome() const;
    double getPreco() const;
    int getQuantidade() const;
    void atualizarInformacoes(const std::string &nome, double preco, int quantidade);
};

}

#endif
