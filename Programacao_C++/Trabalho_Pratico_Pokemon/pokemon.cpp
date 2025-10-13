#include "pokemon.h"

Ataque::Ataque(const std::string& nome, const std::string& categoria, int poder, float precisao, const std::string& tipo)
    : nome(nome), categoria(categoria), poder(poder), precisao(precisao), tipo(tipo) {}

Pokemon::Pokemon(const std::string& nome, const std::string& tipo1, const std::string& tipo2, int hp, int nivel, int ataque, int defesa,
                 int velocidade, int ataqueEspecial, int defesaEspecial)
    : nome(nome), tipo1(tipo1), tipo2(tipo2), hp(hp), nivel(nivel), ataque(ataque), defesa(defesa),
      velocidade(velocidade), ataqueEspecial(ataqueEspecial), defesaEspecial(defesaEspecial) {}

void Pokemon::adicionarAtaque(const Ataque& ataque) {
    ataques.push_back(ataque);
}
