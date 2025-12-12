from datetime import date

# Importa todas as classes necessárias do pacote domain
# Importa as classes de Entidades e Interfaces
from domain.entities import Paciente, Medico, Consulta, TransmissorMS
# Importa as classes de Procedimentos (com a superclasse Procedimento)
from domain.procedures import Faringoplastia, Neurocirurgia, Procedimento
# Importa as Classes de Valor e Entidades Simples
from domain.value_objects import Endereco, Plano, Especialidade, Sala

def run_simulation():
    """
    Simula o fluxo de trabalho do Sistema Hospitalar, 
    demonstrando o uso do Modelo Não Anêmico.
    """
    print("Iniciando a simulação do Sistema Hospitalar (Modelo Não Anêmico)...")

    # 1. Setup de Entidades Simples e Classes de Valor
    endereco_paciente = Endereco("Rua da Paz", 100, "Centro", "38400-000")
    endereco_medico = Endereco("Av. Principal", 50, "Jardins", "38410-000")
    plano_saude = Plano("Plano Ouro", 500.00)
    especialidade_cardio = Especialidade("Cardiologia")

    # 2. Criação das Entidades de Domínio
    paciente1 = Paciente("Maria Silva", date(1980, 5, 15), endereco_paciente, "123.456.789-00", plano_saude)
    medico1 = Medico("Dr. João", date(1975, 1, 1), endereco_medico, 1234, [especialidade_cardio], 150.00)

    # 3. Persistência (Métodos Estáticos de Cadastro nas próprias Entidades - UML)
    Paciente.cadastrar(paciente1)
    Medico.cadastrar(medico1)
    print("-" * 30)

    # 4. Uso dos Comportamentos de Domínio (Marcação)
    consulta_m = Consulta.marcar(medico1, paciente1, date(2025, 12, 15), 300.00)

    # Configuração de Procedimentos
    sala_cirurgia = Sala("Sala 10")
    
    # Procedimento 1: Faringoplastia (corrigido com 'tempo_duracao')
    proc_faringo = Faringoplastia(
        paciente=paciente1, 
        medicos=[medico1], 
        data=date(2026, 1, 10), 
        sala=sala_cirurgia, 
        observacoes="Pronto para cirurgia", 
        valor=5000.00, 
        tempo_duracao=60  # Duração em minutos (Corrigido)
    )
    
    # Procedimento 2: Neurocirurgia (corrigido com 'tempo_duracao')
    proc_neuro = Neurocirurgia(
        paciente=paciente1, 
        medicos=[medico1], 
        data=date(2026, 2, 20), 
        sala=sala_cirurgia, 
        observacoes="Procedimento complexo", 
        valor=15000.00, 
        tempo_duracao=180 # Duração em minutos (Corrigido)
    )

    Procedimento.marcar(medico1, paciente1, date(2026, 1, 10), proc_faringo)
    Procedimento.marcar(medico1, paciente1, date(2026, 2, 20), proc_neuro)
    print("-" * 30)

    # 5. Visualizar Comportamentos (Lógica interna da Entidade)
    print(f"Consultas e Procedimentos de {paciente1.nome}: {len(paciente1.visualizar_consultas())}")
    print(f"Procedimentos de {medico1.nome}: {len(medico1.consultar_procedimentos())}")
    print("-" * 30)

    # 6. Polimorfismo e Lógica de Negócio (Regras na Entidade)
    print(f"Valor Total Faringoplastia (10% copart.): R$ {proc_faringo.calcular_total():.2f}")
    print(f"Valor Total Neurocirurgia (sem copart.): R$ {proc_neuro.calcular_total():.2f}")

    # 7. Interface de Transmissão (Lógica de Negócio)
    transmissor = TransmissorMS()
    transmissor.gerar_dados()
    
if __name__ == "__main__":
    run_simulation()