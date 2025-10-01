import subprocess
import os
import re
import math
import pandas as pd
import matplotlib.pyplot as plt
from tqdm import tqdm

# Configurações do experimento;

PASTA_RELATORIO = "report"
NUMERO_REPETICOES = 10
TAMANHOS_E1 = [100, 200, 400, 800]
TAMANHO_FIXO_E2 = 800
VALORES_P_E2 = [
    # Valores sugeridos pelo PDF: de 1/4 a 1/2 do P padrão, com incrementos
    # Padrão para 800x800 = 80000
    # 1/4 do padrão = 20000
    # 1/2 do padrão = 160000 ... até 32000
    20000, 40000, 60000, 80000, 120000, 160000, 240000, 320000
]

# Funções Auxiliares do Script;
def limpar_resultados_anteriores():
    """Apaga os arquivos de resultado individuais da pasta results"""
    for f in os.listdir("results"):
        if f.startswith("proc_") or f.startswith("thread_"):
            os.remove(os.path.join("results", f))

def ler_tempo_maximo_dos_arquivos(prefixo):
    """
    Lê todos os arquivos com um dado prefixo (proc_ ou thread_) e
    extrai o tempo de execução de cada um e retorna o maior tempo encontrado.
    """
    tempos = []
    arquivos_resultado = [f for f in os.listdir("results") if f.startswith(prefixo)]
    
    for arquivo in arquivos_resultado:
        try:
            with open(os.path.join("results", arquivo), 'r') as f:
                conteudo = f.read()
                match = re.search(r"TEMPO,([\d.]+)", conteudo)
                if match:
                    tempos.append(float(match.group(1)))
        except (IOError, ValueError) as e:
            print(f"  Aviso: Não foi possível ler o tempo do arquivo {arquivo}: {e}")
            
    if not tempos:
        return 0.0

    return max(tempos)

# Funções do experimento

def executar_e1():
    """Experimento E1: Variando o tamanho da matriz"""
    resultados_e1 = []
    
    for tamanho in TAMANHOS_E1:
        print(f"\n  ==== EXPERIMENTO E1: Matrizes {tamanho}x{tamanho} ====")
        
        print(f"Gerando matrizes {tamanho}x{tamanho}")
        subprocess.run(["./bin/auxiliar", str(tamanho), str(tamanho), str(tamanho), str(tamanho)], check=True, capture_output=True)

        p_valor = math.ceil((tamanho * tamanho) / 8)

        tempos_seq = []
        print("Executando algoritmo Sequencial")
        for _ in tqdm(range(NUMERO_REPETICOES), desc="Sequencial"):
            # O tempo do sequencial é medido externamente para manter a consistência
            start_time = pd.Timestamp.now()
            subprocess.run(["./bin/algoritmo_sequencial", "data/m1.txt", "data/m2.txt"], check=True, capture_output=True)
            end_time = pd.Timestamp.now()
            tempos_seq.append((end_time - start_time).total_seconds() * 1000) 
        
        resultados_e1.append({'tamanho': tamanho, 'algoritmo': 'Sequencial', 'tempo': sum(tempos_seq) / len(tempos_seq)})

        # Algoritmos paralelos
        for algoritmo in ['threads', 'process']:
            tempos_paralelo = []
            print(f"Executando algoritmo com {algoritmo.capitalize()} (P={p_valor})")
            for _ in tqdm(range(NUMERO_REPETICOES), desc=algoritmo.capitalize()):
                limpar_resultados_anteriores()
                subprocess.run([f"./bin/algoritmo_paralelo_{algoritmo}", "data/m1.txt", "data/m2.txt", str(p_valor)], check=True, capture_output=True)
                prefixo = "thread_" if algoritmo == "threads" else "proc_"
                tempo_max = ler_tempo_maximo_dos_arquivos(prefixo)
                tempos_paralelo.append(tempo_max)

            resultados_e1.append({'tamanho': tamanho, 'algoritmo': algoritmo.capitalize(), 'tempo': sum(tempos_paralelo) / len(tempos_paralelo)})

    return pd.DataFrame(resultados_e1)

def executar_e2():
    """Experimento E2: Varia o parâmetro P com tamanho de matriz fixo"""
    resultados_e2 = []
    tamanho = TAMANHO_FIXO_E2
    
    print(f"\n ==== EXPERIMENTO E2: Matrizes {tamanho}x{tamanho} (Tamanho Fixo) ====")
    
    print(f"Gerando matrizes {tamanho}x{tamanho}...")
    subprocess.run(["./bin/auxiliar", str(tamanho), str(tamanho), str(tamanho), str(tamanho)], check=True, capture_output=True)

    # Teste dos algoritmos paralelos com diferentes valores de P
    for p_valor in VALORES_P_E2:
        for algoritmo in ['threads', 'process']:
            tempos_paralelo = []
            print(f"Executando com {algoritmo.capitalize()} (P={p_valor})")
            for _ in tqdm(range(NUMERO_REPETICOES), desc=f"{algoritmo.capitalize()} P={p_valor}"):
                limpar_resultados_anteriores()
                subprocess.run([f"./bin/algoritmo_paralelo_{algoritmo}", "data/m1.txt", "data/m2.txt", str(p_valor)], check=True, capture_output=True)
                prefixo = "thread_" if algoritmo == "threads" else "proc_"
                tempo_max = ler_tempo_maximo_dos_arquivos(prefixo)
                tempos_paralelo.append(tempo_max)

            resultados_e2.append({'p_valor': p_valor, 'algoritmo': algoritmo.capitalize(), 'tempo': sum(tempos_paralelo) / len(tempos_paralelo)})

    return pd.DataFrame(resultados_e2)

def gerar_graficos(df_e1, df_e2):
    """Gera e salva os gráficos para os dois experimentos na pasta de relatório"""
    print(f"\n ==== Gerando Gráficos na pasta '{PASTA_RELATORIO}' ====")
    plt.style.use('seaborn-v0_8-whitegrid')

    # Gráfico do Experimento E1
    fig1, ax1 = plt.subplots(figsize=(10, 6))
    
    for algoritmo in df_e1['algoritmo'].unique():
        subset = df_e1[df_e1['algoritmo'] == algoritmo]
        ax1.plot(subset['tamanho'], subset['tempo'], marker='o', linestyle='-', label=algoritmo)
        
    ax1.set_title('Experimento E1: Tempo de Execução vs. Tamanho da Matriz')
    ax1.set_xlabel('Tamanho da Matriz (N x N)')
    ax1.set_ylabel('Tempo Médio de Execução (ms) - Escala Logarítmica')
    ax1.set_yscale('log') 
    ax1.legend()
    ax1.grid(True, which="both", ls="--")
    
    plt.tight_layout()
    caminho_grafico1 = os.path.join(PASTA_RELATORIO, 'grafico_e1.png')
    plt.savefig(caminho_grafico1)
    print(f"Gráfico do Experimento E1 salvo em '{caminho_grafico1}'")
    
    # Gráfico do Experimento E2
    fig2, ax2 = plt.subplots(figsize=(10, 6))
    
    for algoritmo in df_e2['algoritmo'].unique():
        subset = df_e2[df_e2['algoritmo'] == algoritmo]
        ax2.plot(subset['p_valor'], subset['tempo'], marker='x', linestyle='--', label=algoritmo)

    ax2.set_title(f'Experimento E2: Tempo de Execução vs. Parâmetro P (Matriz {TAMANHO_FIXO_E2}x{TAMANHO_FIXO_E2})')
    ax2.set_xlabel('Valor de P (Elementos por Tarefa)')
    ax2.set_ylabel('Tempo Médio de Execução (ms)')
    ax2.legend()
    ax2.grid(True, which="both", ls="--")
    
    plt.tight_layout()
    caminho_grafico2 = os.path.join(PASTA_RELATORIO, 'grafico_e2.png')
    plt.savefig(caminho_grafico2)
    print(f"Gráfico do Experimento E2 salvo em '{caminho_grafico2}'")
    plt.show()

if __name__ == "__main__":
   
# Bloco para garantir caso o make all não tenha sido executado antes
    print("Verificando e compilando os programas C++")
    try:
        subprocess.run(["make", "all"], check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print("ERRO DURANTE A COMPILAÇÃO:")
        print(e.stderr)
        exit(1)
    
    os.makedirs(PASTA_RELATORIO, exist_ok=True)
    
    dados_e1 = executar_e1()
    dados_e2 = executar_e2()

    caminho_csv1 = os.path.join(PASTA_RELATORIO, "resultados_e1.csv")
    caminho_csv2 = os.path.join(PASTA_RELATORIO, "resultados_e2.csv")
    dados_e1.to_csv(caminho_csv1, index=False)
    dados_e2.to_csv(caminho_csv2, index=False)
    print(f"\nDados brutos dos experimentos salvos em '{caminho_csv1}' e '{caminho_csv2}'")

    gerar_graficos(dados_e1, dados_e2)
