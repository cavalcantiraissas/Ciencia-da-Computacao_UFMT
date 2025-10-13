/*
Universidade Federal de Mato Grosso
Disciplina: Programação II
Aluna: Raissa Cavalcanti
*/

// Primeiro Exercício : Simulação ”Yu-Gi-Oh!”

#include <iostream>
#include <string>
#include <map>

using namespace std;

class Card {
protected: 
    string name;
    string type;
public:
    Card(string n = "", string t = "") : name(n), type(t) {}
    string getName() const { return name; }
    string getType() const { return type; }

    friend Card operator+(const Card &c1, const Card &c2);
};

class MonsterCard : public Card {
public:
    MonsterCard(string n) : Card(n, "Monstro") {}
};

class SpellCard : public Card {
public:
    SpellCard(string n) : Card(n, "Magia") {}
};

class EquipCard : public Card {
public:
    EquipCard(string n) : Card(n, "Equipamento") {}
};

// Mapa que define as fusões
map<pair<string, string>, Card> fusionMap;

void initializeFusionMap() {
    fusionMap[make_pair("Koumori Dragon", "Saggi the Dark Clown")] = Card("Reaper of the Cards", "Monstro");
    fusionMap[make_pair("Beast Fangs", "Beast Fangs")] = Card("Eatgaboon", "Armadilha");
    fusionMap[make_pair("Dark Magic", "Dragon")] = Card("Blackland Fire Dragon", "Monstro");
    fusionMap[make_pair("Dragon", "Machine")] = Card("Metal Dragon", "Monstro");
    fusionMap[make_pair("Dark Spellcaster", "Ryu-Kishin")] = Card("Ryu-Kishin Powered", "Monstro");
    fusionMap[make_pair("Dragon Capture Jar", "Mooyan Curry")] = Card("Dragon Capture Jar", "Magia");
    fusionMap[make_pair("Hinotama", "Hinotama")] = Card("Final Flame", "Magia");
    fusionMap[make_pair("Dark Energy", "Acid Trap Hole")] = Card("Dark Hole", "Magia");
    fusionMap[make_pair("Electro-whip", "Metalmorph")] = Card("Raigeki", "Magia");
    fusionMap[make_pair("Goblin’s Secret Remedy", "Goblin’s Secret Remedy")] = Card("Soul of the Pure", "Armadilha");
    fusionMap[make_pair("Legendary Sword", "Sword of Dark Destruction")] = Card("Kunai with Chain", "Equipamento");
    fusionMap[make_pair("Legendary Sword", "Sparks")] = Card("Salamandra", "Equipamento");
    fusionMap[make_pair("Machine Conversion Factory", "Silver Bow & Arrow")] = Card("Winged Trumpeter", "Equipamento");
    fusionMap[make_pair("Castle of Dark Illusions", "Dark-Piercing Light")] = Card("Bright Castle", "Equipamento");
    fusionMap[make_pair("Elegant Egotist", "Horn of the Unicorn")] = Card("Curse of Tri-Horned Dragon", "Monstro");
}

// Sobrecarga do operador +
Card operator+(const Card &c1, const Card &c2) {
    pair<string, string> combo1 = make_pair(c1.getName(), c2.getName());
    pair<string, string> combo2 = make_pair(c2.getName(), c1.getName());

    if (fusionMap.find(combo1) != fusionMap.end()) {
        return fusionMap[combo1];
    } else if (fusionMap.find(combo2) != fusionMap.end()) {
        return fusionMap[combo2];
    } else {
        return Card("Fusão Inválida", "Desconhecido");
    }
}

int main() {
    // Inicializa o mapa de fusões
    initializeFusionMap();

    // Exemplo de fusões
    MonsterCard monstro1("Koumori Dragon");
    MonsterCard monstro2("Saggi the Dark Clown");
    SpellCard magia1("Dragon Capture Jar");
    SpellCard magia2("Mooyan Curry");
    EquipCard equip1("Legendary Sword");
    EquipCard equip2("Sword of Dark Destruction");

    Card fusao1 = monstro1 + monstro2;
    Card fusao2 = magia1 + magia2;
    Card fusao3 = equip1 + equip2;

    cout << "Resultado da Fusão 1: " << fusao1.getName() << " (" << fusao1.getType() << ")\n";
    cout << "Resultado da Fusão 2: " << fusao2.getName() << " (" << fusao2.getType() << ")\n";
    cout << "Resultado da Fusão 3: " << fusao3.getName() << " (" << fusao3.getType() << ")\n";

    return 0;
}
