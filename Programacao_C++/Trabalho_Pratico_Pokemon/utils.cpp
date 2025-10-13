#include "utils.h"
#include "pokemon.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <unordered_set> 
#include <unordered_map>
using namespace std;
 
const unordered_map<string, unordered_map<string, float>> tipoEfetividade = {
    {"Fogo", {{"Normal", 1.0}, {"Fogo", 0.5}, {"Água", 2.0}, {"Elétrico", 1.0}, {"Planta", 0.5}, {"Gelo", 1.0}, {"Lutador", 1.0}, {"Venenoso", 1.0}, {"Terrestre", 2.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 0.5}, {"Rocha", 2.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 0.5}, {"Fada", 0.5}}},
    {"Normal", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 1.0}, {"Gelo", 1.0}, {"Lutador", 2.0}, {"Venenoso", 1.0}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 1.0}, {"Fantasma", 0.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Água", {{"Normal", 1.0}, {"Fogo", 0.5}, {"Água", 0.5}, {"Elétrico", 2.0}, {"Planta", 2.0}, {"Gelo", 1.0}, {"Lutador", 1.0}, {"Venenoso", 1.0}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 0.5}, {"Fada", 1.0}}},
    {"Elétrico", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 0.5}, {"Planta", 1.0}, {"Gelo", 1.0}, {"Lutador", 1.0}, {"Venenoso", 1.0}, {"Terrestre", 2.0}, {"Voador", 0.5}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 0.5}, {"Fada", 1.0}}},
    {"Planta", {{"Normal", 1.0}, {"Fogo", 2.0}, {"Água", 0.5}, {"Elétrico", 0.5}, {"Planta", 0.5}, {"Gelo", 2.0}, {"Lutador", 1.0}, {"Venenoso", 2.0}, {"Terrestre", 0.5}, {"Voador", 2.0}, {"Psíquico", 1.0}, {"Inseto", 2.0}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Gelo", {{"Normal", 1.0}, {"Fogo", 2.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 1.0}, {"Gelo", 0.5}, {"Lutador", 2.0}, {"Venenoso", 1.0}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 2.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 2.0}, {"Fada", 1.0}}},
    {"Lutador", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 1.0}, {"Gelo", 1.0}, {"Lutador", 1.0}, {"Venenoso", 1.0}, {"Terrestre", 1.0}, {"Voador", 2.0}, {"Psíquico", 2.0}, {"Inseto", 0.5}, {"Rocha", 0.5}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Venenoso", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 0.5}, {"Gelo", 1.0}, {"Lutador", 0.5}, {"Venenoso", 0.5}, {"Terrestre", 2.0}, {"Voador", 1.0}, {"Psíquico", 2.0}, {"Inseto", 0.5}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 0.5}}},
    {"Terrestre", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 2.0}, {"Elétrico", 0.0}, {"Planta", 2.0}, {"Gelo", 2.0}, {"Lutador", 1.0}, {"Venenoso", 0.5}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 0.5}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Voador", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 2.0}, {"Planta", 0.5}, {"Gelo", 2.0}, {"Lutador", 0.5}, {"Venenoso", 1.0}, {"Terrestre", 0.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 0.5}, {"Rocha", 2.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 2.0}}},
    {"Psíquico", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 1.0}, {"Gelo", 1.0}, {"Lutador", 0.5}, {"Venenoso", 1.0}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 0.5}, {"Inseto", 2.0}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Inseto", {{"Normal", 1.0}, {"Fogo", 2.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 0.5}, {"Gelo", 1.0}, {"Lutador", 0.5}, {"Venenoso", 1.0}, {"Terrestre", 0.5}, {"Voador", 2.0}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 2.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Rocha", {{"Normal", 0.5}, {"Fogo", 0.5}, {"Água", 2.0}, {"Elétrico", 1.0}, {"Planta", 2.0}, {"Gelo", 1.0}, {"Lutador", 2.0}, {"Venenoso", 0.5}, {"Terrestre", 2.0}, {"Voador", 0.5}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 1.0}, {"Metal", 2.0}, {"Fada", 1.0}}},
    {"Fantasma", {{"Normal", 0.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 1.0}, {"Gelo", 1.0}, {"Lutador", 0.0}, {"Venenoso", 0.5}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 0.5}, {"Rocha", 1.0}, {"Fantasma", 2.0}, {"Dragão", 1.0}, {"Metal", 1.0}, {"Fada", 1.0}}},
    {"Dragão", {{"Normal", 1.0}, {"Fogo", 0.5}, {"Água", 0.5}, {"Elétrico", 0.5}, {"Planta", 0.5}, {"Gelo", 2.0}, {"Lutador", 1.0}, {"Venenoso", 1.0}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 1.0}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 2.0}, {"Metal", 1.0}, {"Fada", 2.0}}},
    {"Metal", {{"Normal", 0.5}, {"Fogo", 2.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 0.5}, {"Gelo", 0.5}, {"Lutador", 1.0}, {"Venenoso", 1.0}, {"Terrestre", 2.0}, {"Voador", 0.5}, {"Psíquico", 0.5}, {"Inseto", 0.5}, {"Rocha", 0.5}, {"Fantasma", 1.0}, {"Dragão", 0.5}, {"Metal", 0.5}, {"Fada", 0.5}}},
    {"Fada", {{"Normal", 1.0}, {"Fogo", 1.0}, {"Água", 1.0}, {"Elétrico", 1.0}, {"Planta", 1.0}, {"Gelo", 1.0}, {"Lutador", 0.5}, {"Venenoso", 2.0}, {"Terrestre", 1.0}, {"Voador", 1.0}, {"Psíquico", 1.0}, {"Inseto", 0.5}, {"Rocha", 1.0}, {"Fantasma", 1.0}, {"Dragão", 0.0}, {"Metal", 2.0}, {"Fada", 1.0}}}
};


void carregarPokemons(std::vector<Pokemon>& pokemons, int quantidade) {
    std::ifstream file("pokemons.txt");
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo pokemons.txt\n";
        return;
    }

    std::string line;
    std::vector<Pokemon> allPokemons;

    getline(file, line);
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        std::stringstream ss(line);
        std::string nome, tipo1, tipo2;
        int hp, nivel, ataque, defesa, velocidade, ataqueEspecial, defesaEspecial;

        getline(ss, nome, ',');
        getline(ss, tipo1, ',');
        getline(ss, tipo2, ',');
        
        std::string hpStr, nivelStr, ataqueStr, defesaStr, velocidadeStr, ataqueEspecialStr, defesaEspecialStr;
        getline(ss, hpStr, ',');
        getline(ss, nivelStr, ',');
        getline(ss, ataqueStr, ',');
        getline(ss, defesaStr, ',');
        getline(ss, velocidadeStr, ',');
        getline(ss, ataqueEspecialStr, ',');
        getline(ss, defesaEspecialStr, ',');

        try {
            hp = std::stoi(hpStr);
            nivel = std::stoi(nivelStr);
            ataque = std::stoi(ataqueStr);
            defesa = std::stoi(defesaStr);
            velocidade = std::stoi(velocidadeStr);
            ataqueEspecial = std::stoi(ataqueEspecialStr);
            defesaEspecial = std::stoi(defesaEspecialStr);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Erro ao converter dados do Pokémon: " << line << std::endl;
            continue;
        }

        Pokemon p(nome, tipo1, tipo2, hp, nivel, ataque, defesa, velocidade, ataqueEspecial, defesaEspecial);
        allPokemons.push_back(p);
    }
    file.close();

    if (allPokemons.size() < quantidade) {
        std::cerr << "Erro: Não há Pokémons suficientes para seleção.\n";
        exit(1);
    }

    std::shuffle(allPokemons.begin(), allPokemons.end(), std::default_random_engine(std::random_device{}()));
    pokemons.assign(allPokemons.begin(), allPokemons.begin() + quantidade);
}

void carregarAtaques(std::vector<Ataque>& ataques, Pokemon& pokemon) {
    std::ifstream file("ataques.txt");
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo ataques.txt\n";
        return;
    }

    std::string line;
    std::vector<Ataque> allAtaques;

    getline(file, line);

    while (getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string nome, categoria, tipo;
        int poder;
        float precisao;

        getline(ss, nome, ',');
        getline(ss, categoria, ',');
        ss >> poder;
        ss.ignore(); 
        ss >> precisao;
        ss.ignore(); 
        getline(ss, tipo, ',');

        if (tipo == "Normal" || tipo == pokemon.tipo1 || tipo == pokemon.tipo2) {
            allAtaques.emplace_back(nome, categoria, poder, precisao, tipo);
        }
    }
    file.close();

    std::shuffle(allAtaques.begin(), allAtaques.end(), std::default_random_engine(std::random_device{}()));
    std::unordered_set<std::string> ataquesEscolhidos;

    for (const auto& ataque : allAtaques) {
        if (ataquesEscolhidos.size() >= 4) break;
        if (ataquesEscolhidos.insert(ataque.nome).second) {
            pokemon.ataques.push_back(ataque);
        }
    }

    if (pokemon.ataques.empty()) {
        std::cerr << "Aviso: Nenhum ataque carregado para o Pokémon " << pokemon.nome << "\n";
    } else {
        std::cout << "Ataques carregados para " << pokemon.nome << ":\n";
        for (const auto& ataque : pokemon.ataques) {
            std::cout << "- " << ataque.nome << " (" << ataque.tipo << ", Poder: " << ataque.poder << ")\n";
        }
    }
}

float calcularDano(const Pokemon& atacante, const Ataque& ataque, const Pokemon& defensor) {
    int level = atacante.nivel;
    int ataqueStat = (ataque.categoria == "Fisico") ? atacante.ataque : atacante.ataqueEspecial;
    int defesaStat = (ataque.categoria == "Fisico") ? defensor.defesa : defensor.defesaEspecial;

    float STAB = (ataque.tipo == atacante.tipo1 || ataque.tipo == atacante.tipo2) ? 1.5 : 1.0;

    float type1 = 1.0, type2 = 1.0;
    if (tipoEfetividade.count(ataque.tipo) > 0) {
        if (tipoEfetividade.at(ataque.tipo).count(defensor.tipo1) > 0) {
            type1 = tipoEfetividade.at(ataque.tipo).at(defensor.tipo1);
        }
        if (!defensor.tipo2.empty() && tipoEfetividade.at(ataque.tipo).count(defensor.tipo2) > 0) {
            type2 = tipoEfetividade.at(ataque.tipo).at(defensor.tipo2);
        }
    }

    if (type1 * type2 == 0) {
        std::cout << "O ataque não é efetivo!\n";
        return 0;
    }

    float critical = (rand() % 16 == 0) ? 2.0 : 1.0;
    if (critical == 2.0) std::cout << "Foi um golpe crítico!\n";

    int randomFactor = 217 + (rand() % (255 - 217 + 1));
    float dano = (((2 * level * ataque.poder * ataqueStat / defesaStat) / 50 + 2) 
                   * critical * STAB * type1 * type2 * (randomFactor / 255.0));
    
    return dano;
}



bool ataqueBemSucedido(const Ataque& ataque) {
    float probabilidadeFalha = 1.0f - ataque.precisao;
    if (static_cast<float>(rand()) / RAND_MAX < probabilidadeFalha) {
        std::cout << "O ataque falhou!\n";
        return false;
    }
    return true;
}
