from typing import List

# Classe de Valor (Address)
class Endereco:
    def __init__(self, logradouro: str, numero: int, bairro: str, cep: str):
        self._logradouro = logradouro
        self._numero = numero
        self._bairro = bairro
        self._cep = cep
    
    @property
    def logradouro(self):
        return self._logradouro

    def __str__(self):
        return f"{self._logradouro}, {self._numero}, {self._bairro} - CEP: {self._cep}"

# Entidade Simples (Specialty)
class Especialidade:
    def __init__(self, nome: str):
        self._nome = nome

    @property
    def nome(self):
        return self._nome
    
    def __eq__(self, other):
        if isinstance(other, Especialidade):
            return self._nome == other.nome
        return False
    
    def __hash__(self):
        return hash(self._nome)

# Entidade Simples (Health Plan)
class Plano:
    def __init__(self, nome: str, mensalidade: float):
        self._nome = nome
        self._mensalidade = mensalidade
    
    @property
    def nome(self):
        return self._nome

# Entidade Simples (Room)
class Sala:
    def __init__(self, nome: str):
        self._nome = nome