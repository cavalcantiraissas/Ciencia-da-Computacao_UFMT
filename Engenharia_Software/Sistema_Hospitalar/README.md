#  Sistema Hospitalar - Estudo de Caso POO

Este projeto implementa o Estudo de Caso do Domínio do Sistema Hospitalar, seguindo os princípios de Programação Orientada a Objetos (POO), com foco na modelagem de domínio.

## Arquitetura Adotada: Modelo Não Anêmico (Domain Model)

A implementação segue o **Modelo Não Anêmico** (também conhecido como *Domain Model*).

### Justificativa da Arquitetura

Nesta abordagem, as classes de domínio são **ricas em comportamento**, não se limitando a serem apenas estruturas de dados (como seria no Modelo Anêmico).

| Classe | Lógica de Negócio (Comportamento) Implementada | Princípio OO Aplicado |
| :--- | :--- | :--- |
| `Paciente` | `visualizar_consultas()` | Encapsulamento e Riqueza do Domínio |
| `Faringoplastia` | `calcular_total()` (Inclui a regra de 10% de coparticipação) | Herança e Polimorfismo |
| `Neurocirurgia` | `calcular_total()` (Retorna o valor base, sem coparticipação) | Herança e Polimorfismo |
| `Consulta` | `marcar()` (Cria a consulta e estabelece a associação com o paciente) | Lógica Agrupada na Entidade |

O código adere a este modelo ao manter os métodos de manipulação e as regras de negócio intrínsecas (como cálculos e validações) dentro das classes que representam o conceito, garantindo uma coesão mais forte.

