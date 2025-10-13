#include "jogo.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <string> 

const std::string caminhoArquivoRanking = "ranking.txt";
int clamp(int valor, int min, int max) {
    return std::max(min, std::min(max, valor));
}

Jogo::Jogo(const std::string& nickname) 
    : nickname(nickname), 
      pontuacao(0), 
      usuarioJogador(nickname) { 
    carregarRanking();    
    carregarPokemons(jogadorPokemons);
    carregarPokemons(cpuPokemons);

    std::vector<Ataque> ataques;
    for (auto& p : jogadorPokemons) carregarAtaques(ataques, p);
    for (auto& p : cpuPokemons) carregarAtaques(ataques, p);
}

void Jogo::exibirMenu() {
    int opcao;
    do {
        std::cout << "\nMenu:\n1. Iniciar Batalha\n2. Selecionar Dificuldade\n3. Exibir Ranking\n4. Sair\nEscolha uma opção: ";
        std::cin >> opcao;

        switch (opcao) {
            case 1: iniciarBatalha();
                break;
            case 2: selecionarDificuldade(); 
                break;
            case 3: exibirRanking(); 
                break;
            case 4: 
                std::cout << "Saindo do jogo...\n"; 
                salvarRanking(caminhoArquivoRanking); 
                break;
            default: 
                std::cout << "Opção inválida! Tente novamente.\n";
        }
    } while (opcao != 4);
}


void Jogo::selecionarDificuldade() {
    std::cout << "Selecione a dificuldade:\n1. Fácil\n2. Médio\n3. Difícil\nEscolha uma opção: ";
    int opcao;
    std::cin >> opcao;

    switch (opcao) {
        case 1: dificuldade = Facil; break;
        case 2: dificuldade = Medio; break;
        case 3: dificuldade = Dificil; break;
        default: std::cout << "Opção inválida! Dificuldade definida como Fácil.\n"; dificuldade = Facil;
    }
}

void Jogo::iniciarBatalha() {
    std::cout << "\nIniciando a batalha...\n";
    escolherPokemonInicial();

    carregarPokemons(cpuPokemons);
    std::vector<Ataque> ataques;
    for (auto& p : cpuPokemons) carregarAtaques(ataques, p);

    while (!jogadorPokemons.empty() && !cpuPokemons.empty()) {
        executarTurnoJogador();
        if (!cpuPokemons.empty()) executarTurnoCPU();
    }

    if (jogadorPokemons.empty()) {
        std::cout << "Você perdeu a batalha!\n";
        atualizarRanking(nickname, pontuacao, false); 
    } else {
        std::cout << "Parabéns! Você venceu a batalha!\n";
        int pontosGanhos = (dificuldade == Facil) ? 10 : (dificuldade == Medio) ? 20 : 30;
        pontuacao += pontosGanhos; 
        atualizarRanking(nickname, pontuacao, true); 
    }

    salvarRanking(caminhoArquivoRanking); 
}


void Jogo::escolherPokemonInicial() {
    std::cout << "\nEscolha seu Pokémon inicial:\n";
    for (size_t i = 0; i < jogadorPokemons.size(); i++) {
        std::cout << i + 1 << ". " << jogadorPokemons[i].nome << " (HP: " << jogadorPokemons[i].hp << ")\n";
    }

    int escolha;
    std::cin >> escolha;
    escolha = clamp(escolha - 1, 0, static_cast<int>(jogadorPokemons.size() - 1));
    std::swap(jogadorPokemons[0], jogadorPokemons[escolha]);
}


void Jogo::executarTurnoJogador() {
    if (jogadorPokemons.empty()) return;

    Pokemon& jogadorPokemon = jogadorPokemons[0];
    Pokemon& cpuPokemon = cpuPokemons[0];

    std::cout << "\nSeu Pokémon: " << jogadorPokemon.nome << " (HP: " << jogadorPokemon.hp << ") enfrenta " 
              << cpuPokemon.nome << " (HP: " << cpuPokemon.hp << ")\n";
    std::cout << "Escolha uma ação:\n1. Atacar\n2. Trocar Pokémon\n";
    
    int acao;
    std::cin >> acao;

    if (acao == 1) {
        executarAtaque(jogadorPokemon, cpuPokemon);
    } else if (acao == 2) {
        escolherNovoPokemon();
    }

    if (jogadorPokemon.hp <= 0) {
        std::cout << "Seu Pokémon " << jogadorPokemon.nome << " foi derrotado!\n";
        jogadorPokemons.erase(jogadorPokemons.begin());

        if (!jogadorPokemons.empty()) {
            escolherNovoPokemon();
        } else {
            std::cout << "Todos os seus Pokémon foram derrotados! Você perdeu a batalha.\n";
        }
    }
}

void Jogo::executarTurnoCPU() {
    if (cpuPokemons.empty()) return;

    Pokemon& cpuPokemon = cpuPokemons[0];
    Pokemon& jogadorPokemon = jogadorPokemons[0];

    std::cout << "\nA CPU está atacando seu Pokémon: " << jogadorPokemon.nome << " (HP: " << jogadorPokemon.hp << ")\n";

    const Ataque& ataqueEscolhido = escolherAtaqueCPU(cpuPokemon);
    if (!ataqueBemSucedido(ataqueEscolhido)) return;

    float dano = calcularDano(cpuPokemon, ataqueEscolhido, jogadorPokemon);
    if (dano > 0) {
        std::cout << "A CPU usou " << ataqueEscolhido.nome << " e causou " << dano << " de dano em " << jogadorPokemon.nome << "!\n";
        jogadorPokemon.hp -= dano;
    } else {
        std::cout << "O ataque da CPU não teve efeito em " << jogadorPokemon.nome << "!\n";
    }

    std::cout << "HP de " << jogadorPokemon.nome << " após o ataque: " << std::max(0, jogadorPokemon.hp) << "\n";

    if (jogadorPokemon.hp <= 0) {
        std::cout << "Seu Pokémon " << jogadorPokemon.nome << " foi derrotado!\n";
        jogadorPokemons.erase(jogadorPokemons.begin());

        if (!jogadorPokemons.empty()) escolherNovoPokemon();
    }
}

void Jogo::escolherNovoPokemon() {
    std::cout << "\nEscolha um novo Pokémon para a batalha:\n";
    for (size_t i = 0; i < jogadorPokemons.size(); i++) {
        std::cout << (i + 1) << ". " << jogadorPokemons[i].nome << " (HP: " << jogadorPokemons[i].hp << ")\n";
    }

    int escolha;
    std::cin >> escolha;

    if (escolha < 1 || escolha > static_cast<int>(jogadorPokemons.size())) {
        std::cout << "Escolha inválida! Tente novamente.\n";
        return;
    }

    std::swap(jogadorPokemons[0], jogadorPokemons[escolha - 1]);
    std::cout << "Você escolheu: " << jogadorPokemons[0].nome << " para continuar a batalha!\n";
}

void Jogo::atualizarRanking(const std::string& jogador, int pontuacao, bool vitoria) {
    std::string jogadorLower = jogador;
    std::transform(jogadorLower.begin(), jogadorLower.end(), jogadorLower.begin(), ::tolower);  // Converte para minúsculas

    auto it = ranking.find(jogadorLower);

    if (it != ranking.end()) {
        if (vitoria) {
            it->second.vitorias++;
            it->second.pontuacao += pontuacao;
        } else {
            it->second.derrotas++;
        }
    } else {
        Jogador novoJogador;
        novoJogador.vitorias = vitoria ? 1 : 0;
        novoJogador.derrotas = vitoria ? 0 : 1;
        novoJogador.pontuacao = vitoria ? pontuacao : 0;
        ranking[jogadorLower] = novoJogador;
    }

    std::vector<std::pair<std::string, Jogador>> rankingOrdenado(ranking.begin(), ranking.end());
    
    std::sort(rankingOrdenado.begin(), rankingOrdenado.end(), [](const std::pair<std::string, Jogador>& a, const std::pair<std::string, Jogador>& b) {
        if (a.second.pontuacao == b.second.pontuacao) {
            return a.second.vitorias > b.second.vitorias;  
        }
        return a.second.pontuacao > b.second.pontuacao;  
    });

    ranking.clear();  
    for (const auto& entry : rankingOrdenado) {
        ranking[entry.first] = entry.second; 
    }
    salvarRanking(caminhoArquivoRanking);
}

void Jogo::salvarRanking(const std::string& caminhoArquivo) {
    std::ofstream arquivo(caminhoArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de ranking para salvar." << std::endl;
        return;
    }

    for (auto it = ranking.begin(); it != ranking.end(); ++it) {
        const std::string& nickname = it->first;
        const Jogador& jogador = it->second;
        arquivo << nickname << " "
                << jogador.pontuacao << " "
                << jogador.vitorias << " "
                << jogador.derrotas << std::endl;
    }

    arquivo.close();
    std::cout << "Ranking salvo com sucesso em " << caminhoArquivo << "." << std::endl;
}

void Jogo::carregarRanking() {
    std::ifstream arquivo("ranking.txt");
    if (arquivo.is_open()) {
        while (!arquivo.eof()) {
            Usuario usuario = Usuario::carregar(arquivo);
            if (!arquivo.fail()) {
                ranking[usuario.getNickname()] = {usuario.getPontuacao(), usuario.getVitorias(), usuario.getDerrotas()};
            }
        }
    } else {
        std::cerr << "Erro ao carregar o ranking.\n";
    }
}

void Jogo::executarAtaque(Pokemon& atacante, Pokemon& defensor) {
    if (atacante.ataques.empty()) {
        std::cerr << "Erro: O Pokémon " << atacante.nome << " não possui ataques carregados.\n";
        return;
    }

    std::cout << "Escolha um ataque:\n";
    for (size_t i = 0; i < atacante.ataques.size(); i++) {
        std::cout << i + 1 << ". " << atacante.ataques[i].nome 
                  << " (" << atacante.ataques[i].poder << " poder, " 
                  << atacante.ataques[i].precisao * 100 << "% precisão)\n";
    }

    int escolha;
    std::cin >> escolha;
    escolha = clamp(escolha - 1, 0, static_cast<int>(atacante.ataques.size() - 1));
    const Ataque& ataqueEscolhido = atacante.ataques[escolha];

    if (!ataqueBemSucedido(ataqueEscolhido)) return;

    float dano = calcularDano(atacante, ataqueEscolhido, defensor);
    if (dano > 0) {
        std::cout << "Você usou " << ataqueEscolhido.nome << " e causou " << dano 
                  << " de dano em " << defensor.nome << "!\n";
        defensor.hp -= dano;
    } else {
        std::cout << "O ataque não teve efeito em " << defensor.nome << "!\n";
    }

    std::cout << "HP de " << defensor.nome << " após o ataque: " 
              << std::max(0, defensor.hp) << "\n";

    if (defensor.hp <= 0) {
        std::cout << "O Pokémon " << defensor.nome << " foi derrotado!\n";
        cpuPokemons.erase(cpuPokemons.begin());
    }
}

const Ataque& Jogo::escolherAtaqueCPU(const Pokemon& cpuPokemon) {
    if (dificuldade == Facil) {
        return *std::min_element(cpuPokemon.ataques.begin(), cpuPokemon.ataques.end(),
                                 [](const Ataque& a, const Ataque& b) { return a.poder < b.poder; });
    } else if (dificuldade == Dificil) {
        return *std::max_element(cpuPokemon.ataques.begin(), cpuPokemon.ataques.end(),
                                 [](const Ataque& a, const Ataque& b) { return a.poder < b.poder; });
    } else {
        return cpuPokemon.ataques[rand() % cpuPokemon.ataques.size()];
    }
}

void Jogo::salvarPontuacaoUsuario(const std::string& nickname) {
    std::ofstream arquivo("ranking.txt", std::ios::app);
    if (arquivo.is_open()) {
        arquivo << nickname << " " << pontuacao << std::endl;
        arquivo.close();
        std::cout << "Pontuação salva para o usuário: " << nickname << std::endl;
    } else {
        std::cerr << "Erro ao abrir o arquivo de ranking." << std::endl;
    }
}

void Jogo::carregarOuCriarUsuario(const std::string& nickname) {
    std::ifstream arquivo("ranking.txt");
    std::string nome;
    int pontuacaoLida;
    bool usuarioEncontrado = false;

    if (arquivo.is_open()) {
        while (arquivo >> nome >> pontuacaoLida) {
            std::transform(nome.begin(), nome.end(), nome.begin(), ::tolower);

            if (nome == nickname) {
                this->pontuacao = pontuacaoLida;
                usuarioEncontrado = true;
                std::cout << "Usuário " << nickname << " carregado com pontuação: " << pontuacao << std::endl;
                break;
            }
        }
        arquivo.close();

        if (!usuarioEncontrado) {
            this->pontuacao = 0;  
            std::cout << "Novo usuário " << nickname << " criado com pontuação inicial: " << pontuacao << std::endl;
        }
    } else {
        std::cerr << "Erro ao abrir o arquivo de ranking." << std::endl;
    }
}


void Jogo::exibirRanking() {
    std::cout << "Ranking de Pontuações:\n";

    std::vector<std::pair<std::string, Jogador>> rankingOrdenado(ranking.begin(), ranking.end());

    std::sort(rankingOrdenado.begin(), rankingOrdenado.end(),
              [](const std::pair<std::string, Jogador>& a, const std::pair<std::string, Jogador>& b) {
                  if (a.second.pontuacao == b.second.pontuacao) {
                      return a.second.vitorias > b.second.vitorias;
                  }
                  return a.second.pontuacao > b.second.pontuacao;
              });

    for (const auto& entry : rankingOrdenado) {
        const auto& jogador = entry.second;
        std::cout << "Jogador: " << entry.first
                  << ", Pontuação: " << jogador.pontuacao
                  << ", Vitórias: " << jogador.vitorias
                  << ", Derrotas: " << jogador.derrotas << std::endl;
    }
}
