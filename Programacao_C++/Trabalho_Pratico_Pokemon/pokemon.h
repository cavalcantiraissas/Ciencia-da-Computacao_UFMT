#ifndef POKEMON_H
#define POKEMON_H

#include <string>
#include <vector>

class Ataque {
public:
    std::string nome;
    std::string categoria;
    int poder;
    float precisao;
    std::string tipo;

    Ataque(const std::string& nome, const std::string& categoria, int poder, float precisao, const std::string& tipo);
};

class Pokemon {
public:
    std::string nome;
    std::string tipo1;
    std::string tipo2;
    int hp;
    int nivel;
    int ataque;
    int defesa;
    int velocidade;
    int ataqueEspecial;
    int defesaEspecial;
    std::vector<Ataque> ataques;

    Pokemon(const std::string& nome, const std::string& tipo1, const std::string& tipo2, int hp, int nivel, int ataque, int defesa,
            int velocidade, int ataqueEspecial, int defesaEspecial);
    void adicionarAtaque(const Ataque& ataque);
};

#endif
