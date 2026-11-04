# Atividades de Classe: Programação em Java

**Exercícios de Java feitos em aula: fundamentos, orientação a objetos, exceções, coleções, arquivos e as primeiras interfaces gráficas com Swing.**

[![Java](https://img.shields.io/badge/Java-8%2B-ED8B00?logo=openjdk&logoColor=white)](https://openjdk.org/)
[![Disciplina](https://img.shields.io/badge/disciplina-Programa%C3%A7%C3%A3o_em_Java-6f42c1)](../)

Atividades da disciplina de Programação III (UFMT). Cada arquivo é um programa independente.

---

## Conteúdo

### Fundamentos e exceções

| Arquivo | O que faz |
|---|---|
| [Calculadora.java](Calculadora.java) | Calculadora com as operações básicas |
| [CalculadoraSimples.java](CalculadoraSimples.java) | Calculadora pelo terminal |
| [CalculadoraRaiz.java](CalculadoraRaiz.java) | Raiz quadrada com tratamento da entrada |
| [Contador.java](Contador.java) | Contador simples |
| [Fibonacci.java](Fibonacci.java) | Sequência de Fibonacci |
| [ClasseEleitoral.java](ClasseEleitoral.java) | Indica se o voto é facultativo ou obrigatório a partir da idade |
| [ConversorStringParaInt.java](ConversorStringParaInt.java) | Conversão de `String` para `int` tratando `NumberFormatException` |
| [DivisaoSegura.java](DivisaoSegura.java) | Divisão que trata a divisão por zero |

### Orientação a objetos

| Arquivo | O que faz |
|---|---|
| [Carro.java](Carro.java), [Livro.java](Livro.java) | Classes com atributos e métodos |
| [Main_Pessoa.java](Main_Pessoa.java) | Herança: `Aluno` estende `Pessoa` e usa `super()` |
| [Main_Animal.java](Main_Animal.java) | Polimorfismo: `Cachorro` e `Gato` sobrescrevem `emitirSom()` |
| [ContaBancariaDetalhada.java](ContaBancariaDetalhada.java) | Conta bancária com histórico de operações |

### Coleções e arquivos

| Arquivo | O que faz |
|---|---|
| [CadastroPessoas.java](CadastroPessoas.java) | Cadastro de pessoas numa coleção |
| [ListaAlunos.java](ListaAlunos.java) | Lista de alunos |
| [GerenciadorNumeros.java](GerenciadorNumeros.java) | Operações sobre uma coleção de números |
| [GerenciadorPalavras.java](GerenciadorPalavras.java) | Palavras únicas digitadas, guardadas num `HashSet<String>` |
| [LeitorArquivoTexto.java](LeitorArquivoTexto.java) | Leitura de arquivo texto |
| [GravadorArquivo.java](GravadorArquivo.java) | Escrita em arquivo |
| [AnalisadorArquivoSimples.java](AnalisadorArquivoSimples.java) | Usa `File` para ver existência, tipo, tamanho e caminho de um arquivo ou pasta |

### Interfaces gráficas (Swing)

| Arquivo | O que faz |
|---|---|
| [JanelaBoasVindas.java](JanelaBoasVindas.java) | `JFrame` com mensagem de boas-vindas e botão para fechar |
| [JanelaLogin.java](JanelaLogin.java) | Tela de login com `JTextField`, `JPasswordField` e `JOptionPane` |

## Como executar

```bash
javac Fibonacci.java
java Fibonacci
```

---

[← Programação em Java](../) · [Repositório principal](../../)
