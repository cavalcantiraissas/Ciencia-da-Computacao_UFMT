from abc import ABC, abstractmethod
from datetime import date
from typing import List

# Importa as classes de outros arquivos (para evitar dependência circular, Paciente e Medico 
# são tipados apenas para o Procedimento)
from .value_objects import Sala

# Dummy imports para type hinting sem criar dependência circular no runtime
class Paciente: pass
class Medico: pass

# Classe Abstrata (Procedure)
class Procedimento(ABC):
    def __init__(self, paciente: Paciente, medicos: List[Medico], data: date, sala: Sala, observacoes: str, valor: float, tempo_duracao: int):
        self._paciente = paciente
        self._medicos = medicos
        self._data = data
        self._sala = sala
        self._observacoes = observacoes
        self._valor = valor
        self._tempo_duracao = tempo_duracao
        
    @abstractmethod
    def calcular_total(self) -> float:
        """Calcula o valor total do procedimento, implementado nas subclasses."""
        pass
    
    @staticmethod
    def marcar(medico: Medico, paciente: Paciente, data: date, procedimento):
        # Importa o repositório global
        from .entities import REPOSITORIO_HOSPITALAR
        
        # Lógica de marcação: Atualizar associações e persistir
        procedimento._data = data
        
        # Adicionar o procedimento ao paciente e ao médico (lógica de domínio)
        paciente.add_consulta(procedimento) # Procedimento também é uma 'consulta'
        medico.add_procedimento(procedimento)
        
        REPOSITORIO_HOSPITALAR['procedimentos'].append(procedimento)
        print(f"Procedimento {type(procedimento).__name__} marcado para {data}.")
        
    def get_valor_base(self):
        return self._valor

# Subclasse (Pharyngoplasty)
class Faringoplastia(Procedimento):
    def calcular_total(self) -> float:
        # Lógica de Negócio: 10% de coparticipação
        coparticipacao = self.get_valor_base() * 0.10
        return self.get_valor_base() + coparticipacao

# Subclasse (Neurosurgery)
class Neurocirurgia(Procedimento):
    def calcular_total(self) -> float:
        # Lógica de Negócio: Sem coparticipação
        return self.get_valor_base()