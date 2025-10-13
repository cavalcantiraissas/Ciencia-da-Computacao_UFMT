#ifndef CONSULTA_H
#define CONSULTA_H

#include "Produto.h"
#include <vector>

namespace Consulta {

Cadastro::Produto buscarProduto(int codigo, const std::vector<Cadastro::Produto> &produtos);

}

#endif
