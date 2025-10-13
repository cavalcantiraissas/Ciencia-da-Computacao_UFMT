#include "Consulta.h"
#include <stdexcept>

namespace Consulta {

Cadastro::Produto buscarProduto(int codigo, const std::vector<Cadastro::Produto> &produtos) {
    for (const auto &produto : produtos) {
        if (produto.getCodigo() == codigo) {
            return produto;
        }
    }
    throw std::out_of_range("Produto não encontrado.");
}

}
