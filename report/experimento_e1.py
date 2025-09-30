# Script para testar algoritmos de multiplicação de matrizes;
# Implementa o Experimento E1 do trabalho de Sistemas Operacionais;

import subprocess
import os
import time
import json

# Configurações do experimento;
tamanhos_ = [100, 200, 400, 800, 1600]
vezes_ = 10
algoritmos_ = ['sequencial', 'threads', 'processos']
resultados_ = {}

print("=== EXPERIMENTO E1 ===")
print("Testando algoritmos de multiplicação de matrizes")
print()

# Loop principal do experimento;
for tamanho_ in tamanhos_:
    print(f"Testando matrizes {tamanho_}x{tamanho_}...")
    
    # Gera matrizes de teste;
    print("  Gerando matrizes...")
    comando_gerar_ = ["./bin/auxiliar", str(tamanho_), str(tamanho_), str(tamanho_), str(tamanho_)]
    subprocess.run(comando_gerar_, check=True)
    
    # Testa cada algoritmo;
    for algoritmo_ in algoritmos_:
        print(f"  Testando {algoritmo_}...")
        tempos_ = []
        
        # Executa 10 vezes para calcular média;
        for i_ in range(vezes_):
            print(f"    Execução {i_+1}/{vezes_}: ", end="")
            
            # Limpa resultados anteriores;
            if algoritmo_ == 'threads':
                os.system("rm -f results/thread_*.txt")
            elif algoritmo_ == 'processos':
                os.system("rm -f results/proc_*.txt")
            
            # Monta comando específico;
            if algoritmo_ == 'sequencial':
                comando_ = ["./bin/algoritmo_sequencial", "data/m1.txt", "data/m2.txt"]
            elif algoritmo_ == 'threads':
                comando_ = ["./bin/algoritmo_paralelo_threads", "data/m1.txt", "data/m2.txt", "4"]
            else:  # processos
                comando_ = ["./bin/algoritmo_paralelo_process", "data/m1.txt", "data/m2.txt", "4"]
            
            # Executa e mede tempo;
            inicio_ = time.time()
            try:
                subprocess.run(comando_, capture_output=True, text=True, check=True)
                fim_ = time.time()
                sucesso_ = True
            except subprocess.CalledProcessError:
                fim_ = time.time()
                sucesso_ = False
            
            if sucesso_:
                if algoritmo_ == 'sequencial':
                    tempo_execucao_ = fim_ - inicio_
                else:
                    tempo_execucao_ = ler_tempo_maximo_(algoritmo_)
                
                tempos_.append(tempo_execucao_)
                print(f"{tempo_execucao_:.3f}s")
            else:
                print("ERRO")
        
        # Calcula tempo médio e salva resultado;
        if tempos_:
            tempo_medio_ = sum(tempos_) / len(tempos_)
            print(f"  {algoritmo_}: tempo médio = {tempo_medio_:.3f}s")
            
            if tamanho_ not in resultados_:
                resultados_[tamanho_] = {}
            resultados_[tamanho_][algoritmo_] = tempo_medio_
        else:
            print(f"  {algoritmo_}: FALHOU")
    
    print()

# Salva resultados em JSON;
with open('experimento_e1_results.json', 'w') as f:
    json.dump(resultados_, f, indent=2)

print("=== EXPERIMENTO CONCLUÍDO ===")
print("Resultados salvos em: experimento_e1_results.json")

def ler_tempo_maximo_(algoritmo_):
    """Lê o tempo máximo dos arquivos de resultado dos algoritmos paralelos;"""
    if algoritmo_ == 'threads':
        arquivos_ = os.listdir('results')
        arquivos_thread_ = [f for f in arquivos_ if f.startswith('thread_')]
    else:  # processos
        arquivos_ = os.listdir('results')
        arquivos_thread_ = [f for f in arquivos_ if f.startswith('proc_')]
    
    tempo_maximo_ = 0
    for arquivo_ in arquivos_thread_:
        try:
            with open(f'results/{arquivo_}', 'r') as f:
                linha_ = f.readline().strip()
                tempo_ = float(linha_)
                if tempo_ > tempo_maximo_:
                    tempo_maximo_ = tempo_
        except:
            pass
    
    return tempo_maximo_
