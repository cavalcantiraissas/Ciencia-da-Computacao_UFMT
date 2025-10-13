#ifndef ELETRONICO_H
#define ELETRONICO_H

#include "Item.h"

class Eletronico : public Item {
public:
    Eletronico(int codigo, double preco, int qtdEstoque);
    // Métodos específicos de Eletronico
};

#endif
