"""
FrotaGesta — ui/validacoes.py
Funções de validação de CPF e placa usadas nos formulários.
"""
import re


def validar_cpf(cpf: str) -> bool:
    numeros = re.sub(r"\D", "", cpf)
    if len(numeros) != 11:
        return False
    if len(set(numeros)) == 1:
        return False
    soma = sum(int(numeros[i]) * (10 - i) for i in range(9))
    resto = (soma * 10) % 11
    if resto == 10:
        resto = 0
    if resto != int(numeros[9]):
        return False
    soma = sum(int(numeros[i]) * (11 - i) for i in range(10))
    resto = (soma * 10) % 11
    if resto == 10:
        resto = 0
    if resto != int(numeros[10]):
        return False
    return True


def formatar_cpf(cpf: str) -> str:
    n = re.sub(r"\D", "", cpf)
    if len(n) == 11:
        return f"{n[:3]}.{n[3:6]}.{n[6:9]}-{n[9:]}"
    return cpf


def validar_placa(placa: str) -> bool:
    p = placa.strip().upper().replace("-", "")
    padrao_antigo   = re.fullmatch(r"[A-Z]{3}\d{4}", p)
    padrao_mercosul = re.fullmatch(r"[A-Z]{3}\d[A-Z]\d{2}", p)
    return bool(padrao_antigo or padrao_mercosul)
