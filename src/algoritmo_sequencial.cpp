#include <iostream>
#include <chrono>
#include <fstream>
#include "matriz_utils.h"

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

// Função para multiplicar duas matrizes sequencialmente
Matriz* multiplicar_matrizes_sequencial(Matriz* A, Matriz* B) {
    if (A->n_colunas != B->n_linhas) {
        std::cerr << "Erro: Dimensões incompatíveis para multiplicação" << std::endl;
        return nullptr;
    }
    
    Matriz* C = criar_matriz(A->n_linhas, B->n_colunas);
    if (C == nullptr) {
        return nullptr;
    }
    
    // Inicializar matriz resultado com zeros
    for (int i = 0; i < C->n_linhas; i++) {
        for (int j = 0; j < C->n_colunas; j++) {
            C->dados[i][j] = 0.0;
        }
    }
    
    // Multiplicação sequencial
    for (int i = 0; i < A->n_linhas; i++) {
        for (int j = 0; j < B->n_colunas; j++) {
            for (int k = 0; k < A->n_colunas; k++) {
                C->dados[i][j] += A->dados[i][k] * B->dados[k][j];
            }
        }
    }
    
    return C;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_matriz1> <arquivo_matriz2>" << std::endl;
        return 1;
    }
    
    std::string arquivo_m1 = argv[1];
    std::string arquivo_m2 = argv[2];
    
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
              << " e " << m2->n_linhas << "x" << m2->n_colunas << " sequencialmente..." << std::endl;
    
    // Medir tempo de execução
    auto inicio = std::chrono::high_resolution_clock::now();
    
    Matriz* resultado = multiplicar_matrizes_sequencial(m1, m2);
    
    auto fim = std::chrono::high_resolution_clock::now();
    auto duracao = std::chrono::duration_cast<std::chrono::milliseconds>(fim - inicio);
    
    if (resultado == nullptr) {
        std::cerr << "Erro na multiplicação" << std::endl;
        liberar_matriz(m1);
        liberar_matriz(m2);
        return 1;
    }
    
    // Salvar resultado
    std::string arquivo_resultado = "results/resultado_sequencial.txt";
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
