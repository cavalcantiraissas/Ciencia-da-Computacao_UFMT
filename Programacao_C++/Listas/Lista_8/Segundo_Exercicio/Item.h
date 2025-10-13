#ifndef ITEM_H
#define ITEM_H

#include <string>

class Item {
protected:
    int codigo;
    double preco;
    int qtdEstoque;

public:
    Item(int codigo, double preco, int qtdEstoque);
    virtual ~Item() = default;
    int getCodigo() const;
    double getPreco() const;
    int getQtdEstoque() const;
    void vender(int qtd);
};

#endif
