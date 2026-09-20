#ifndef JOGO_H
#define JOGO_H

#include <string>
#include <vector>
#include <map>
#include <istream>
#include "pokemon.h"

enum Dificuldade { Facil, Medio, Dificil };

struct Jogador {
    int pontuacao;
    int vitorias;
    int derrotas;
};

class Usuario {
public:
    Usuario() : nickname(""), pontuacao(0), vitorias(0), derrotas(0) {}
    explicit Usuario(const std::string& nickname) : nickname(nickname), pontuacao(0), vitorias(0), derrotas(0) {}

    static Usuario carregar(std::istream& arquivo) {
        Usuario usuario;
        arquivo >> usuario.nickname >> usuario.pontuacao >> usuario.vitorias >> usuario.derrotas;
        return usuario;
    }

    const std::string& getNickname() const { return nickname; }
    int getPontuacao() const { return pontuacao; }
    int getVitorias() const { return vitorias; }
    int getDerrotas() const { return derrotas; }

private:
    std::string nickname;
    int pontuacao;
    int vitorias;
    int derrotas;
};

class Jogo {
public:
    explicit Jogo(const std::string& nickname);

    void exibirMenu();
    void selecionarDificuldade();
    void iniciarBatalha();
    void escolherPokemonInicial();
    void executarTurnoJogador();
    void executarTurnoCPU();
    void escolherNovoPokemon();
    void atualizarRanking(const std::string& jogador, int pontosGanhos, bool vitoria);
    void salvarRanking(const std::string& caminhoArquivo);
    void carregarRanking();
    void executarAtaque(Pokemon& atacante, Pokemon& defensor);
    const Ataque& escolherAtaqueCPU(const Pokemon& cpuPokemon);
    void salvarPontuacaoUsuario(const std::string& nickname);
    void carregarOuCriarUsuario(const std::string& nickname);
    void exibirRanking();

private:
    std::string nickname;
    int pontuacao;
    Usuario usuarioJogador;
    std::vector<Pokemon> jogadorPokemons;
    std::vector<Pokemon> cpuPokemons;
    Dificuldade dificuldade = Facil;
    std::map<std::string, Jogador> ranking;
};

#endif
