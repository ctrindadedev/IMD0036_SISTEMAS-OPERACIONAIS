#include <iostream>
#include <chrono>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <string> 
#include <cmath>  
#include <iomanip> 
#include "matriz_utils.h"
struct DadosThread {
    Matriz* A;
    Matriz* B;
    Matriz* C;
    int linha_inicio;
    int linha_fim;
    int thread_id;
};

// Função executada por cada thread
void* thread_multiplicacao(void* arg) {
    DadosThread* dados = (DadosThread*)arg;
    
    std::cout << "Thread " << dados->thread_id << " processando linhas " << dados->linha_inicio 
              << " a " << dados->linha_fim - 1 << std::endl;
    
    //  Medir o tempo de execução DENTRO da thread
    auto start_time = std::chrono::high_resolution_clock::now();

    // Multiplicar faixa de linhas 
    for (int i = dados->linha_inicio; i < dados->linha_fim; i++) {
        for (int j = 0; j < dados->B->n_colunas; j++) {
            dados->C->dados[i][j] = 0.0;
            for (int k = 0; k < dados->A->n_colunas; k++) {
                dados->C->dados[i][j] += dados->A->dados[i][k] * dados->B->dados[k][j];
            }
        }
    }
    
    //Finalizar a medição de tempo
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> tempo_execucao = end_time - start_time;

    std::string nome_arq_saida = "results/thread_" + std::to_string(dados->thread_id) + ".txt";
    std::ofstream arq_saida(nome_arq_saida);
    if (!arq_saida) {
        std::cerr << "Erro ao criar arquivo de resultado para a thread " << dados->thread_id << std::endl;
    } else {
        arq_saida << std::fixed << std::setprecision(5);

        // Escreve as linhas calculadas por esta thread no arquivo
        for (int i = dados->linha_inicio; i < dados->linha_fim; i++) {
            for (int j = 0; j < dados->C->n_colunas; j++) {
                arq_saida << dados->C->dados[i][j] << " ";
            }
            arq_saida << "\n";
        }
        // Adiciona o tempo de execução da thread no final do arquivo
        arq_saida << "TEMPO," << tempo_execucao.count() << "\n";
        arq_saida.close();
    }

    std::cout << "Thread " << dados->thread_id << " concluída" << std::endl;
    return nullptr;
}

// Função principal de multiplicação paralela com threads
Matriz* multiplicar_matrizes_paralelo_threads(Matriz* A, Matriz* B, int num_threads) {
    if (A->n_colunas != B->n_linhas) {
        std::cerr << "Erro: Dimensões incompatíveis para multiplicação" << std::endl;
        return nullptr;
    }
    
    Matriz* C = criar_matriz(A->n_linhas, B->n_colunas);
    if (C == nullptr) {
        return nullptr;
    }
    
    // Calcular quantas linhas cada thread vai processar
    int linhas_por_thread = A->n_linhas / num_threads;
    int linhas_restantes = A->n_linhas % num_threads;
    
    std::vector<pthread_t> threads(num_threads);
    std::vector<DadosThread> dados_threads(num_threads);
    
    int linha_atual = 0;
    
    // Criar threads
    for (int i = 0; i < num_threads; i++) {
        int linhas_para_esta_thread = linhas_por_thread + (i < linhas_restantes ? 1 : 0);
        int linha_fim = linha_atual + linhas_para_esta_thread;
        
        // Configurar dados da thread
        dados_threads[i].A = A;
        dados_threads[i].B = B;
        dados_threads[i].C = C;
        dados_threads[i].linha_inicio = linha_atual;
        dados_threads[i].linha_fim = linha_fim;
        dados_threads[i].thread_id = i;
        
        // Criar thread
        if (pthread_create(&threads[i], nullptr, thread_multiplicacao, &dados_threads[i]) != 0) {
            std::cerr << "Erro ao criar thread " << i << std::endl;
            return nullptr;
        }
        
        std::cout << "Criada thread " << i << " para processar linhas " << linha_atual 
                  << " a " << linha_fim - 1 << std::endl;
        
        linha_atual = linha_fim;
    }
    
    // Aguardar todas as threads terminarem
    for (int i = 0; i < num_threads; i++) {
        if (pthread_join(threads[i], nullptr) != 0) {
            std::cerr << "Erro ao aguardar thread " << i << std::endl;
        } else {
            std::cout << "Thread " << i << " terminou" << std::endl;
        }
    }
    
    return C;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_matriz1> <arquivo_matriz2> <P>" << std::endl;
        return 1;
    }
    
    std::string arquivo_m1 = argv[1];
    std::string arquivo_m2 = argv[2];
    int p = std::atoi(argv[3]); 
    
    if (p <= 0) {
        std::cerr << "P (elementos por tarefa) deve ser positivo" << std::endl;
        return 1;
    }

    // Carregar matrizes
    Matriz* m1 = ler_matriz_arquivo(arquivo_m1);
    if (m1 == nullptr) {
        std::cerr << "Erro ao carregar matriz 1" << std::endl;
        return 1;
    }
    
    Matriz* m2 = ler_matriz_arquivo(arquivo_m2);
    if (m2 == nullptr) {
        std::cerr << "Erro ao carregar matriz 2" << std::endl;
        liberar_matriz(m1);
        return 1;
    }
    
    // Calcular o número de threads com base em 'P' e no total de elementos, pelo calculo do número de threads necessário para cobrir todas as linhas
    long long total_elementos = (long long)m1->n_linhas * m2->n_colunas;
    int num_threads = std::ceil((double)total_elementos / p);

    
    std::cout << "Multiplicando matrizes " << m1->n_linhas << "x" << m1->n_colunas 
              << " e " << m2->n_linhas << "x" << m2->n_colunas 
              << " com " << num_threads << " threads..." << std::endl;
    
    
    Matriz* resultado = multiplicar_matrizes_paralelo_threads(m1, m2, num_threads);
    
    if (resultado == nullptr) {
        std::cerr << "Erro na multiplicação" << std::endl;
        liberar_matriz(m1);
        liberar_matriz(m2);
        return 1;
    }
    
    std::cout << "Processo concluido. Verifique os arquivos individuais na pasta 'results/'." << std::endl;
    
    // Liberar memória
    liberar_matriz(m1);
    liberar_matriz(m2);
    liberar_matriz(resultado);
    
    return 0;
}