"""Camada de blocos — o único acesso permitido à imagem.

Todo o vsfs passa por read_block/write_block. Não existe seek arbitrário, não
existe "leia 100 bytes a partir do offset 4237". Discos transferem blocos, e é
essa restrição que mantém o modelo de custo realista: dá para contar quantas
operações de I/O cada chamada de sistema custa, e comparar com a Figura 40.3 do
capítulo.

É a versão de brinquedo da camada de blocos do Linux. Um driver de sistema de
arquivos dentro do kernel também não chama read() — ele pede blocos.

Este módulo não importa layout.py de propósito: a camada de blocos não conhece
a geometria do vsfs, só o tamanho do bloco que lhe passarem.
"""

import os


class Disk:
    """Acesso em blocos a uma imagem de disco já existente.

    Abre, não cria. Criar a imagem é trabalho do mkfs — manter essa fronteira
    evita que um caminho errado produza silenciosamente um arquivo vazio.
    """

    def __init__(self, caminho, block_size):
        tamanho = os.path.getsize(caminho)

        if tamanho == 0:
            raise ValueError(f"{caminho}: imagem vazia; rode o mkfs primeiro")
        if tamanho % block_size != 0:
            raise ValueError(
                f"{caminho}: {tamanho} bytes não é múltiplo do bloco "
                f"({block_size}); imagem truncada ou corrompida"
            )

        self.caminho = caminho
        self.block_size = block_size
        self.num_blocks = tamanho // block_size
        self.fd = os.open(caminho, os.O_RDWR)

        # Contadores de I/O. Duas linhas agora, e é o instrumento que permite
        # traçar o custo de cada chamada de sistema quando elas existirem.
        self.reads = 0
        self.writes = 0

    def read_block(self, n):
        """Lê o bloco n inteiro. Devolve exatamente block_size bytes."""
        self._valida_numero(n)

        # os.pread recebe o offset como argumento e não mexe na posição do
        # arquivo. Um seek() seguido de read() seriam duas operações sobre um
        # offset compartilhado — e o FUSE roda multithread por padrão, o que
        # produziria leituras no lugar errado de forma intermitente.
        dados = os.pread(self.fd, self.block_size, n * self.block_size)

        if len(dados) != self.block_size:
            raise IOError(
                f"leitura curta no bloco {n}: {len(dados)} de {self.block_size} bytes"
            )

        self.reads += 1
        return dados

    def write_block(self, n, dados):
        """Grava o bloco n inteiro. Exige exatamente block_size bytes."""
        self._valida_numero(n)

        # A validação mais importante do módulo. Um pwrite de 80 bytes tem
        # sucesso e deixa os outros 4016 com o conteúdo velho — nada falha,
        # nada avisa, e a corrupção só aparece muito depois.
        if len(dados) != self.block_size:
            raise ValueError(
                f"write_block({n}): recebeu {len(dados)} bytes, "
                f"esperava exatamente {self.block_size}"
            )

        os.pwrite(self.fd, dados, n * self.block_size)
        self.writes += 1

    def _valida_numero(self, n):
        if not isinstance(n, int) or not 0 <= n < self.num_blocks:
            raise ValueError(
                f"bloco {n} fora da imagem (válidos: 0 a {self.num_blocks - 1})"
            )

    def close(self):
        if self.fd is not None:
            os.close(self.fd)
            self.fd = None

    def __enter__(self):
        return self

    def __exit__(self, *_):
        self.close()
        return False
