# Trabalho de Monitoria — Organização Hierárquica de Dados

**Disciplina:** Estrutura de Dados I  
**Professor:** Ivairton M. Santos  
**Monitora:** Beatriz  

---

## Descrição Geral

Este projeto tem como objetivo **fixar e aplicar os conceitos de Árvores Binárias e Árvores com Múltiplos Filhos (N-árias)** em um contexto prático de organização e consulta de dados reais.

O sistema foi desenvolvido em **linguagem C**, e manipula dados do **Amazon Product Dataset 2023**, simulando a organização hierárquica de categorias e produtos com base em relações entre elementos de uma base de dados real.

---

## Objetivo do Trabalho

Construir um sistema capaz de:

- **Organizar categorias de produtos** em uma estrutura hierárquica (árvore N-ária);
- **Armazenar produtos** em uma estrutura de árvore binária, ordenados pelo preço;
- **Integrar as duas estruturas**, permitindo consultas eficientes entre categorias e produtos;
- **Oferecer funcionalidades interativas** para exploração dos dados via menu.

---

## Base de Dados

Dataset utilizado:  
📊 [Amazon Product Dataset 2023 (Kaggle)](https://www.kaggle.com/datasets/asaniczka/amazon-products-dataset-2023-1-4m-products/data)

Arquivos necessários:
- `amazon_categories.csv`
- `amazon_products.csv`

> ⚠️ **Observação importante:**  
> Os arquivos CSV originais são muito grandes para hospedagem no GitHub.  
> Portanto, é necessário **baixá-los manualmente** e adicioná-los na pasta do projeto antes de executar o programa.
---

##  Estrutura do Sistema

### Parte 1 — Hierarquia de Categorias (Árvore N-ária)
Criação de uma estrutura hierárquica para as **270 categorias** disponíveis no arquivo `amazon_categories.csv`.  
Cada categoria é associada a uma das áreas principais:

- Diverso (raiz)
  - Casa  
  - Pessoal  
    - Homem  
    - Mulher  
    - Criança  
    - Bebê  
  - Automotivo  
  - Saúde  
  - Eletrônicos  
    - Computador  
    - Celular  
    - Videogame  
  - Educação  
  - Diversão  
  - Viagem  
  - Pet  
  - Esporte  

> Categorias que não se enquadrarem em nenhuma área devem ser associadas à categoria “Diverso”.

---

### Parte 2 — Estrutura de Produtos (Árvore Binária)
Cada produto do arquivo `amazon_products.csv` é armazenado em uma **árvore binária de busca (BST)**, utilizando o **preço** como chave principal.

Campos armazenados:
- Código (ASIN)
- Título
- Avaliação (Stars)
- Preço
- Categoria associada

---

### Parte 3 — Integração das Estruturas
Integração entre a **árvore N-ária de categorias** e a **árvore binária de produtos**, permitindo:

- Consultar produtos por categoria;
- Identificar a categoria correspondente de um produto;
- Relacionar hierarquias e subníveis de categorias com seus produtos.

---

### Parte 4 — Funcionalidades do Sistema
O sistema apresenta um **menu interativo** com as seguintes opções:

1.  **Listar produtos por faixa de preço** (entre valores *x* e *y* informados pelo usuário);
2.  **Listar produtos por categoria** (selecionada pelo usuário);
3.  **Listar os 5 produtos mais caros** de cada categoria;
4.  **Listar os 5 produtos mais baratos** de cada categoria.


---

 🧠 Conceitos Fundamentais Envolvidos

- Árvores binárias de busca (BST)  
- Árvores N-árias (com múltiplos filhos)  
- Alocação dinâmica de memória  
- Integração de estruturas complexas  
- Manipulação e leitura de arquivos CSV  
- Análise e otimização de algoritmos  

---

## Autoria

**Discente:** Raissa Cavalcanti  
**Disciplina:** Estrutura de Dados I — UFMT  
📧 [cavalcanti.c.raissa@gmail.com](mailto:cavalcanti.c.raissa@gmail.com)  
🔗 [LinkedIn](https://www.linkedin.com/in/cavalcantiraissa/)

---

## ⭐ Observações Finais

Este trabalho foi desenvolvido com fins **acadêmicos e de aprendizado prático**, aplicando conceitos de Estrutura de Dados em um problema real de **organização hierárquica e integração de informações**.

> ⚠️ **Importante:**  
> Os arquivos `.csv` devem ser **baixados manualmente** e adicionados na pasta para execução correta do programa, pois o tamanho do dataset excede o limite de upload do GitHub.

