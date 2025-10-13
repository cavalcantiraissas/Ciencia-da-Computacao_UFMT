#include <iostream>
#include "Produto.h"
#include "Consulta.h"
#include <vector>

int main() {
    std::vector<Cadastro::Produto> estoque;

    try {
        Cadastro::Produto produto1(1, "Produto A", 10.0, 100);
        Cadastro::Produto produto2(2, "Produto B", 20.0, 50);
        estoque.push_back(produto1);
        estoque.push_back(produto2);

        Cadastro::Produto produto = Consulta::buscarProduto(1, estoque);
        std::cout << "Produto encontrado: " << produto.getNome() << std::endl;

    } catch (const std::invalid_argument &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    } catch (const std::out_of_range &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }

    return 0;
}
