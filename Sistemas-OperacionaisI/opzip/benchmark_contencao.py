#!/usr/bin/env python3

import subprocess
import time
import statistics
import json

ARQUIVO = "bench_contencao.txt"
REPETICOES = 5
BLOCO = 4096  # bloco pequeno -> muitos blocos -> muitas operacoes de lock


def medir(binario, t, k, repeticoes=REPETICOES):
    tempos = []
    for _ in range(repeticoes):
        inicio = time.perf_counter()
        subprocess.run(
            ["./" + binario, "-t", str(t), "-b", str(BLOCO), "-k", str(k), ARQUIVO],
            stdout=subprocess.DEVNULL,
            check=True,
        )
        tempos.append(time.perf_counter() - inicio)
    return statistics.median(tempos), min(tempos), max(tempos)


def main():
    resultados = {}
    for threads in [2, 4, 8, 16, 32]:
        k = max(2 * threads, 1)
        med_global, min_g, max_g = medir("opzip", threads, k)
        med_posicao, min_p, max_p = medir("opzip_por_posicao", threads, k)
        resultados[threads] = {
            "mutex_global": med_global,
            "mutex_por_posicao": med_posicao,
        }
        print(f"t={threads:3d} k={k:3d}  "
              f"mutex_global={med_global:.4f}s (min {min_g:.4f} max {max_g:.4f})  "
              f"mutex_por_posicao={med_posicao:.4f}s (min {min_p:.4f} max {max_p:.4f})")

    with open("resultados_contencao.json", "w") as f:
        json.dump(resultados, f, indent=2)

    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    threads_x = list(resultados.keys())
    y_global = [resultados[t]["mutex_global"] for t in threads_x]
    y_posicao = [resultados[t]["mutex_por_posicao"] for t in threads_x]

    plt.figure(figsize=(6, 4.5))
    plt.plot(threads_x, y_global, "o-", label="Mutex global (opzip.c entregue)", color="#2b6cb0")
    plt.plot(threads_x, y_posicao, "s-", label="Mutex por posicao (variante)", color="#38a169")
    plt.xlabel("Numero de threads produtoras (-t)")
    plt.ylabel("Tempo de execucao (s)")
    plt.title("Contencao: mutex global vs mutex por posicao\n(bloco pequeno = 4 KB, alta taxa de lock)")
    plt.legend()
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig("contencao_mutex.png", dpi=150)
    plt.close()
    print("Grafico salvo: contencao_mutex.png")


if __name__ == "__main__":
    main()
