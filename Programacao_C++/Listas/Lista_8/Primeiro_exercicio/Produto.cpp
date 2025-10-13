#include "Produto.h"
#include <stdexcept>

namespace Cadastro {

Produto::Produto(int codigo, const std::string &nome, double preco, int quantidade) 
    : codigo(codigo), nome(nome), preco(preco), quantidade(quantidade) {
    if (codigo <= 0 || nome.empty()) {
        throw std::invalid_argument("Código inválido ou nome vazio.");
    }
}

int Produto::getCodigo() const { return codigo; }
std::string Produto::getNome() const { return nome; }
double Produto::getPreco() const { return preco; }
int Produto::getQuantidade() const { return quantidade; }

void Produto::atualizarInformacoes(const std::string &nome, double preco, int quantidade) {
    if (nome.empty()) {
        throw std::invalid_argument("Nome não pode ser vazio.");
    }
    this->nome = nome;
    this->preco = preco;
    this->quantidade = quantidade;
}

}
