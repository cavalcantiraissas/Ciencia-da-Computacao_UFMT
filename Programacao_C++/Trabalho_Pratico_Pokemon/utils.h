#ifndef UTILS_H
#define UTILS_H

#include "pokemon.h"
#include <vector>

void carregarPokemons(std::vector<Pokemon>& pokemons, int quantidade = 3);
void carregarAtaques(std::vector<Ataque>& ataques, Pokemon& pokemon);
float calcularDano(const Pokemon& atacante, const Ataque& ataque, const Pokemon& defensor);
bool ataqueBemSucedido(const Ataque& ataque);

#endif
