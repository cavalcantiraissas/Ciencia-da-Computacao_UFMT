from datetime import date
from typing import List, Optional
from abc import ABC, abstractmethod

# Importa as classes de outros arquivos do domínio
from .value_objects import Endereco, Plano, Sala, Especialidade
from .procedures import Procedimento, Faringoplastia, Neurocirurgia # Necessário para type hinting

# Simulação de Persistência Global
REPOSITORIO_HOSPITALAR = {
    'pacientes': {},
    'medicos': {},
    'consultas': [],
    'procedimentos': []
}

# Superclasse/Classe Abstrata (Person)
class Pessoa:
    def __init__(self, nome: str, data_nascimento: date, endereco: Endereco):
        self._nome = nome
        self._data_nascimento = data_nascimento
        self._endereco = endereco
    
    @property
    def nome(self):
        return self._nome

    @property
    def endereco(self):
        return self._endereco
    
    def alterar_endereco(self, novo_endereco: Endereco):
        self._endereco = novo_endereco
        
    def __str__(self):
        return f"Nome: {self._nome}, Nasc.: {self._data_nascimento}, Endereço: {self._endereco}"

# Entidade (Patient)
class Paciente(Pessoa):
    def __init__(self, nome: str, data_nascimento: date, endereco: Endereco, cpf: str, plano: Plano):
        super().__init__(nome, data_nascimento, endereco)
        self._cpf = cpf
        self._plano = plano
        self._consultas: List[Consulta] = [] # Tipo de referência cruzada
    
    @staticmethod
    def cadastrar(paciente):
        # Lógica de persistência
        REPOSITORIO_HOSPITALAR['pacientes'][paciente._cpf] = paciente
        print(f"Paciente {paciente.nome} cadastrado com sucesso.")

    def visualizar_consultas(self):
        # Comportamento: O paciente deve conseguir visualizar suas consultas
        return self._consultas
    
    def add_consulta(self, consulta):
        self._consultas.append(consulta)

    @property
    def cpf(self):
        return self._cpf
    
# Entidade (Doctor)
class Medico(Pessoa):
    def __init__(self, nome: str, data_nascimento: date, endereco: Endereco, crm: int, especialidades: List[Especialidade], valor_hora: float):
        super().__init__(nome, data_nascimento, endereco)
        self._crm = crm
        self._especialidades = especialidades
        self._valor_hora = valor_hora
        self._procedimentos: List[Procedimento] = [] # Tipo de referência cruzada

    @staticmethod
    def cadastrar(medico):
        REPOSITORIO_HOSPITALAR['medicos'][medico._crm] = medico
        print(f"Médico {medico.nome} cadastrado com sucesso.")

    def consultar_procedimentos(self):
        # Comportamento: os médicos consultarem seus procedimentos
        return self._procedimentos
    
    def add_procedimento(self, procedimento):
        self._procedimentos.append(procedimento)

    @property
    def crm(self):
        return self._crm

# Entidade (Appointment)
class Consulta:
    def __init__(self, paciente: Paciente, medico: Medico, data: date, receituario: Optional[str] = None, valor: float = 0.0):
        self._paciente = paciente
        self._medico = medico
        self._data = data
        self._receituario = receituario
        self._valor = valor
        
    @staticmethod
    def marcar(medico: Medico, paciente: Paciente, data: date, valor: float) -> 'Consulta':
        # Cria a nova instância e estabelece a associação
        nova_consulta = Consulta(paciente, medico, data, valor=valor)
        paciente.add_consulta(nova_consulta) # Associa ao Paciente
        REPOSITORIO_HOSPITALAR['consultas'].append(nova_consulta)
        print(f"Consulta marcada para {paciente.nome} com Dr(a). {medico.nome} em {data}.")
        return nova_consulta

# Interface (Protocol in Python)
class ITransmissaoDadosMinisterioSaude(ABC):
    @abstractmethod
    def gerar_dados(self):
        """Método para repassar procedimentos realizados para o Ministério da Saúde."""
        pass
        
class TransmissorMS(ITransmissaoDadosMinisterioSaude):
    def gerar_dados(self):
        # Acessa a simulação de persistência
        dados_a_transmitir = [
            {'tipo': type(p).__name__, 'valor_total': p.calcular_total()} 
            for p in REPOSITORIO_HOSPITALAR['procedimentos']
        ]
        print(f"\n--- Transmissão de Dados ao Ministério da Saúde ---")
        for dado in dados_a_transmitir:
            print(f"Procedimento: {dado['tipo']}, Valor Total: R$ {dado['valor_total']:.2f}")