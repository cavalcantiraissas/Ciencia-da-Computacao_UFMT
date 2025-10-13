#ifndef VESTUARIO_H
#define VESTUARIO_H

#include "Item.h"

class Vestuario : public Item {
public:
    Vestuario(int codigo, double preco, int qtdEstoque);
    // Métodos específicos de Vestuario
};

#endif
