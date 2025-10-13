/*
Universidade Federal de Mato Grosso
Disciplina : Programação II || Lista 03
Aluna: Raissa Caroline Cavalcanti da Silva

*/

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <algorithm>

struct Usuario {
    std::string nome;
    std::vector<std::weak_ptr<Usuario>> dependencias;
    std::unique_ptr<int> recursoExclusivo;

    Usuario(const std::string& nome) : nome(nome), recursoExclusivo(nullptr) {}
};

std::vector<std::shared_ptr<Usuario>> usuarios;

std::shared_ptr<Usuario> encontrarUsuario(const std::string& nome) {
    auto it = std::find_if(usuarios.begin(), usuarios.end(), [&nome](const std::shared_ptr<Usuario>& usuario) {
        return usuario->nome == nome;
    });
    return (it != usuarios.end()) ? *it : nullptr;
}

void adicionarUsuario(const std::string& nome) {
    if (encontrarUsuario(nome) != nullptr) {
        std::cout << "Usuário '" << nome << "' já existe.\n";
        return;
    }
    usuarios.push_back(std::make_shared<Usuario>(nome));
    std::cout << "Usuário '" << nome << "' adicionado.\n";
}

void adicionarDependencia(const std::string& nomeUsuario, const std::string& nomeDependencia) {
    auto usuario = encontrarUsuario(nomeUsuario);
    auto dependencia = encontrarUsuario(nomeDependencia);

    if (!usuario) {
        std::cout << "Usuário '" << nomeUsuario << "' não encontrado.\n";
        return;
    }
    if (!dependencia) {
        std::cout << "Dependência '" << nomeDependencia << "' não encontrada.\n";
        return;
    }

    // Verificar se a dependência já existe
    for (const auto& weakDep : usuario->dependencias) {
        if (auto dep = weakDep.lock()) {
            if (dep->nome == nomeDependencia) {
                std::cout << "Usuário '" << nomeDependencia << "' já é uma dependência de '" << nomeUsuario << "'.\n";
                return;
            }
        }
    }

    usuario->dependencias.push_back(dependencia);
    std::cout << "Usuário '" << nomeDependencia << "' adicionado como dependência de '" << nomeUsuario << "'.\n";
}

void listarDependencias(const std::string& nomeUsuario) {
    auto usuario = encontrarUsuario(nomeUsuario);

    if (!usuario) {
        std::cout << "Usuário '" << nomeUsuario << "' não encontrado.\n";
        return;
    }

    std::cout << "Dependências de '" << nomeUsuario << "':\n";
    for (const auto& weakDep : usuario->dependencias) {
        if (auto dep = weakDep.lock()) {
            std::cout << "- " << dep->nome << "\n";
        }
    }
}

void atribuirRecursoExclusivo(const std::string& nomeUsuario, int valor) {
    auto usuario = encontrarUsuario(nomeUsuario);

    if (!usuario) {
        std::cout << "Usuário '" << nomeUsuario << "' não encontrado.\n";
        return;
    }

    usuario->recursoExclusivo = std::unique_ptr<int>(new int(valor));
    std::cout << "Recurso exclusivo de '" << nomeUsuario << "' atribuído com valor: " << valor << ".\n";
}

int main() {
    adicionarUsuario("Alice");
    adicionarUsuario("Bob");
    adicionarUsuario("Carol");

    adicionarDependencia("Alice", "Bob");
    adicionarDependencia("Alice", "Carol");

    atribuirRecursoExclusivo("Alice", 42);

    listarDependencias("Alice");
    listarDependencias("Bob");

    return 0;
}
