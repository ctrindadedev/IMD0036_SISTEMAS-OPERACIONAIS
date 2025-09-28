#include <iostream>
#include <chrono>
#include <fstream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>
#include "matriz_utils.h"

// Estrutura para compartilhar dados entre processos
struct DadosCompartilhados {
    int n_linhas_A;
    int n_colunas_A;
    int n_linhas_B;
    int n_colunas_B;
    int n_linhas_C;
    int n_colunas_C;
    int linha_inicio;
    int linha_fim;
    int processo_id;
};

// Função para carregar matriz de arquivo
Matriz* carregar_matriz_arquivo(const std::string& nome_arquivo) {
    std::ifstream arquivo(nome_arquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir arquivo: " << nome_arquivo << std::endl;
        return nullptr;
    }
    
    int linhas, colunas;
    arquivo >> linhas >> colunas;
    
    Matriz* m = criar_matriz(linhas, colunas);
    if (m == nullptr) {
        std::cerr << "Erro ao criar matriz" << std::endl;
        return nullptr;
    }
    
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            arquivo >> m->dados[i][j];
        }
    }
    
    arquivo.close();
    return m;
}

// Função para multiplicar uma faixa de linhas da matriz
void multiplicar_faixa_linhas(Matriz* A, Matriz* B, Matriz* C, int linha_inicio, int linha_fim) {
    for (int i = linha_inicio; i < linha_fim; i++) {
        for (int j = 0; j < B->n_colunas; j++) {
            C->dados[i][j] = 0.0;
            for (int k = 0; k < A->n_colunas; k++) {
                C->dados[i][j] += A->dados[i][k] * B->dados[k][j];
            }
        }
    }
}

// Função executada por cada processo filho
void processo_filho(Matriz* A, Matriz* B, Matriz* C, int linha_inicio, int linha_fim, int processo_id) {
    std::cout << "Processo " << processo_id << " processando linhas " << linha_inicio 
              << " a " << linha_fim - 1 << std::endl;
    
    multiplicar_faixa_linhas(A, B, C, linha_inicio, linha_fim);
    
    std::cout << "Processo " << processo_id << " concluído" << std::endl;
    exit(0);
}

// Função principal de multiplicação paralela com processos
Matriz* multiplicar_matrizes_paralelo_processos(Matriz* A, Matriz* B, int num_processos) {
    if (A->n_colunas != B->n_linhas) {
        std::cerr << "Erro: Dimensões incompatíveis para multiplicação" << std::endl;
        return nullptr;
    }
    
    Matriz* C = criar_matriz(A->n_linhas, B->n_colunas);
    if (C == nullptr) {
        return nullptr;
    }
    
    // Calcular quantas linhas cada processo vai processar
    int linhas_por_processo = A->n_linhas / num_processos;
    int linhas_restantes = A->n_linhas % num_processos;
    
    std::vector<pid_t> pids(num_processos);
    int linha_atual = 0;
    
    // Criar processos filhos
    for (int i = 0; i < num_processos; i++) {
        int linhas_para_este_processo = linhas_por_processo + (i < linhas_restantes ? 1 : 0);
        int linha_fim = linha_atual + linhas_para_este_processo;
        
        pid_t pid = fork();
        
        if (pid == 0) {
            // Processo filho
            processo_filho(A, B, C, linha_atual, linha_fim, i);
        } else if (pid > 0) {
            // Processo pai
            pids[i] = pid;
            std::cout << "Criado processo " << i << " com PID " << pid << std::endl;
        } else {
            std::cerr << "Erro ao criar processo filho " << i << std::endl;
            return nullptr;
        }
        
        linha_atual = linha_fim;
    }
    
    // Aguardar todos os processos filhos terminarem
    for (int i = 0; i < num_processos; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        std::cout << "Processo " << i << " (PID " << pids[i] << ") terminou" << std::endl;
    }
    
    return C;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_matriz1> <arquivo_matriz2> <num_processos>" << std::endl;
        return 1;
    }
    
    std::string arquivo_m1 = argv[1];
    std::string arquivo_m2 = argv[2];
    int num_processos = std::atoi(argv[3]);
    
    if (num_processos <= 0) {
        std::cerr << "Número de processos deve ser positivo" << std::endl;
        return 1;
    }
    
    // Carregar matrizes
    Matriz* m1 = carregar_matriz_arquivo(arquivo_m1);
    if (m1 == nullptr) {
        std::cerr << "Erro ao carregar matriz 1" << std::endl;
        return 1;
    }
    
    Matriz* m2 = carregar_matriz_arquivo(arquivo_m2);
    if (m2 == nullptr) {
        std::cerr << "Erro ao carregar matriz 2" << std::endl;
        liberar_matriz(m1);
        return 1;
    }
    
    std::cout << "Multiplicando matrizes " << m1->n_linhas << "x" << m1->n_colunas 
              << " e " << m2->n_linhas << "x" << m2->n_colunas 
              << " com " << num_processos << " processos..." << std::endl;
    
    // Medir tempo de execução
    auto inicio = std::chrono::high_resolution_clock::now();
    
    Matriz* resultado = multiplicar_matrizes_paralelo_processos(m1, m2, num_processos);
    
    auto fim = std::chrono::high_resolution_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
    
    if (resultado == nullptr) {
        std::cerr << "Erro na multiplicação" << std::endl;
        liberar_matriz(m1);
        liberar_matriz(m2);
        return 1;
    }
    
    // Salvar resultado
    std::string arquivo_resultado = "results/resultado_paralelo_processos.txt";
    if (!salvar_matriz_arquivo(resultado, arquivo_resultado)) {
        std::cerr << "Erro ao salvar resultado" << std::endl;
    } else {
        std::cout << "Resultado salvo em " << arquivo_resultado << std::endl;
    }
    
    std::cout << "Tempo de execução: " << duracao.count() << " ms" << std::endl;
    
    // Liberar memória
    liberar_matriz(m1);
    liberar_matriz(m2);
    liberar_matriz(resultado);
    
    return 0;
}

