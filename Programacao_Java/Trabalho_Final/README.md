# Sistema de Gerenciamento de Itens
Um sistema completo em Java para gerenciamento de itens (Livros e Filmes) com interface gráfica, desenvolvido como parte de um trabalho acadêmico.

## Sobre o Projeto
Sistema que permite cadastrar, listar, buscar, exportar e importar informações sobre livros e filmes através de uma interface gráfica amigável.

## Funcionalidades

### Operações Principais

- Gerenciamento de Livros: Cadastro com título, descrição, autor e número de páginas
- Gerenciamento de Filmes: Cadastro com título, descrição, diretor e duração
- Busca Avançada: Filtragem de itens por título
- Relatórios: Contagem de itens por tipo
- Persistência: Exportação e importação de dados
- Listagem Ordenada: Exibição de itens ordenados por título
- Validações

## Arquitetura do Sistema

### Estrutura de Pacotes

```text
src/
├── exceptions/          # Exceções personalizadas
├── model/              # Modelos de dados
├── service/            # Lógica de negócio
└── gui/                # Interface gráfica
```
## Pré-requisitos

- Java JDK 8 ou superior
- Ambiente com suporte a Swing

## Classes e Responsabilidades

### Camada de Apresentação (GUI)

ItemManagerGUI: Interface gráfica principal com Swing
- Formulários para cadastro de livros e filmes
- Área de exibição de resultados
- Controles de busca e navegação
-  Camada de Serviço

### GerenciadorItens: Núcleo do sistema
- CRUD de itens
- Busca e ordenação
- Importação/exportação
- Estatísticas

### Camada de Modelo
- Item: Classe abstrata base
- Livro: Especialização para livros
- Filme: Especialização para filmes

### Camada de Exceções
- CampoVazioException: Campos obrigatórios não preenchidos
- DuplicadoException: Itens com títulos duplicados

### Formato de Dados

#### Estrutura de Exportação
```text
Livro|Título|Descrição|2024-01-15|Autor|300
Filme|Título|Descrição|2024-01-15|Diretor|120
```

Campos por Tipo
Livro:

Título*
Descrição*
Autor*
Número de Páginas*
Filme:

Título*
Descrição*
Diretor*
Duração (minutos)*
*Campos obrigatórios

## Padrões e Boas Práticas
- Herança: Item como classe base abstrata
- Polimorfismo: Métodos exibirDetalhes() e toExportString()
- Encapsulamento: Atributos privados com getters/setters
- Tratamento de Exceções: Exceções personalizadas e específicas
- Interface Gráfica: Separada da lógica de negócio
- Documentação: Código comentado e organizado

##  Fluxo de Dados
```text
Interface → Validação → Serviço → Modelo
    ↑           ↑          ↑        ↑
   Exibição   Exceções  Regras   Persistência
```
 
# Informações
 - Desenvolvedora: Raissa  Cavalcanti 
 - Tecnologias: Java, Swing, Programação Orientada a Objetos
 - Data: 10 de Outubro de 2025

## Licença
- Este projeto foi desenvolvido para fins educacionais como parte de um trabalho acadêmico.

## Exemplo de Uso
- Cadastrar Livro:

Selecione "Livro" no tipo
Preencha título, descrição, autor e páginas
Clique em "Adicionar"
Buscar Item:

Digite parte do título no campo de busca
Clique em "Filtrar"
Exportar Dados:

Clique em "Exportar Dados"
Escolha o local e nome do arquivo
Os dados serão salvos em formato texto
Ver Estatísticas:

Clique em "Listar Todos"
Veja a contagem por tipo no final da listagem
Sistema robusto e intuitivo para gerenciamento completo de acervo de livros e filmes!
