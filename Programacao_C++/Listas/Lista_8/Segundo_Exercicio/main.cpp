#include <iostream>
#include <vector>
#include <memory>
#include "Eletronico.h"
#include "Vestuario.h"

void processarVenda(std::shared_ptr<Item> item, int quantidade) {
    try {
        auto eletr = std::dynamic_pointer_cast<Eletronico>(item);
        if (eletr) {
            eletr->vender(quantidade);
            std::cout << "Venda de eletrônico processada." << std::endl;
        } else {
            auto vest = std::dynamic_pointer_cast<Vestuario>(item);
            if (vest) {
                vest->vender(quantidade);
                std::cout << "Venda de vestuário processada." << std::endl;
            } else {
                throw std::bad_cast();
            }
        }
    } catch (const std::runtime_error &e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    } catch (const std::bad_cast &e) {
        std::cerr << "Erro: casting inválido." << std::endl;
    }
}

int main() {
    std::shared_ptr<Item> item1 = std::make_shared<Eletronico>(1, 100.0, 10);
    std::shared_ptr<Item> item2 = std::make_shared<Vestuario>(2, 50.0, 5);

    processarVenda(item1, 3);
    processarVenda(item2, 6); // Deve lançar uma exceção de estoque insuficiente

    return 0;
}
