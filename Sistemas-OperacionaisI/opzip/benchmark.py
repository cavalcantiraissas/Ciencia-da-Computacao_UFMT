#!/usr/bin/env python3
"""
benchmark.py - mede o desempenho do opzip variando numero de threads (-t)
e tamanho do buffer circular (-k), e gera os graficos usados no relatorio.

Uso: python3 benchmark.py arquivo_teste
"""
import subprocess
import time
import statistics
import sys
import json

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

ARQUIVO = sys.argv[1] if len(sys.argv) > 1 else "bench.txt"
REPETICOES = 3
BLOCO = 1024 * 1024  # 1 MB


def medir(t, k, repeticoes=REPETICOES):
    tempos = []
    for _ in range(repeticoes):
        inicio = time.perf_counter()
        subprocess.run(
            ["./opzip", "-t", str(t), "-b", str(BLOCO), "-k", str(k), ARQUIVO],
            stdout=subprocess.DEVNULL,
            check=True,
        )
        tempos.append(time.perf_counter() - inicio)
    return statistics.median(tempos)


def main():
    resultados = {"threads": {}, "k": {}}

    # Experimento 1: speedup vs numero de threads (K = 2*t em cada ponto)
    lista_threads = [1, 2, 4, 8, 16]
    tempos_threads = {}
    for t in lista_threads:
        k = max(2 * t, 1)
        tempo = medir(t, k)
        tempos_threads[t] = tempo
        print(f"t={t:2d} k={k:3d}  tempo={tempo:.4f}s")

    t_base = tempos_threads[1]
    resultados["threads"] = {
        t: {"tempo": tempos_threads[t], "speedup": t_base / tempos_threads[t]}
        for t in lista_threads
    }

    # Experimento 2: efeito de K, com numero de threads fixo
    t_fixo = 4
    lista_k = [1, 2, 4, 8, 16, 32, 64, 128]
    tempos_k = {}
    for k in lista_k:
        tempo = medir(t_fixo, k)
        tempos_k[k] = tempo
        print(f"[K] t={t_fixo} k={k:3d}  tempo={tempo:.4f}s")
    resultados["k"] = tempos_k

    with open("resultados_benchmark.json", "w") as f:
        json.dump(resultados, f, indent=2)

    # Grafico 1: speedup vs threads 
    threads_x = lista_threads
    speedup_y = [resultados["threads"][t]["speedup"] for t in threads_x]
    ideal_y = threads_x

    plt.figure(figsize=(6, 4.5))
    plt.plot(threads_x, ideal_y, "--", color="gray", label="Speedup ideal (linear)")
    plt.plot(threads_x, speedup_y, "o-", color="#2b6cb0", label="Speedup medido")
    plt.xlabel("Numero de threads produtoras (-t)")
    plt.ylabel("Speedup (T1 / Tn)")
    plt.title("Speedup vs numero de threads")
    plt.legend()
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig("speedup_vs_threads.png", dpi=150)
    plt.close()

    # Grafico 2: tempo vs K 
    k_x = lista_k
    tempo_y = [tempos_k[k] for k in k_x]

    plt.figure(figsize=(6, 4.5))
    plt.plot(k_x, tempo_y, "o-", color="#c05621")
    plt.xscale("log", base=2)
    plt.xlabel("Tamanho do buffer circular K (escala log2)")
    plt.ylabel("Tempo de execucao (s)")
    plt.title(f"Efeito de K no desempenho (t = {t_fixo} threads)")
    plt.grid(alpha=0.3)
    plt.tight_layout()
    plt.savefig("desempenho_vs_k.png", dpi=150)
    plt.close()

    print("\nGraficos salvos: speedup_vs_threads.png, desempenho_vs_k.png")
    print("Dados brutos salvos em resultados_benchmark.json")


if __name__ == "__main__":
    main()
