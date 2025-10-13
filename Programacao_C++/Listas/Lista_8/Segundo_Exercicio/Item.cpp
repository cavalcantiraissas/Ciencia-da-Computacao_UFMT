#include "Item.h"
#include <stdexcept>

Item::Item(int codigo, double preco, int qtdEstoque) 
    : codigo(codigo), preco(preco), qtdEstoque(qtdEstoque) {}

int Item::getCodigo() const { return codigo; }
double Item::getPreco() const { return preco; }
int Item::getQtdEstoque() const { return qtdEstoque; }

void Item::vender(int qtd) {
    if (qtdEstoque < qtd) {
        throw std::runtime_error("Estoque insuficiente.");
    }
    qtdEstoque -= qtd;
}
