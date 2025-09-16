#include "matriz_utils.h"
#include <iostream>  
#include <fstream>    
#include <iomanip>    
#include <random>     
#include <stdexcept>  

Matriz* criar_matriz(int linhas, int colunas) {
        Matriz* m = new Matriz;
        m->n_linhas = linhas;
        m->n_colunas = colunas;
        m->dados.resize(linhas, std::vector<double>(colunas));
        return m;
}

void liberar_matriz(Matriz* m) {
    delete m;
}

void preencher_matriz(Matriz* m) {
    if (m == nullptr) return;
    // Algoritmo gerador de números com c++ de forma moderna, encontrado na internet (Adaptado para gerar só números de 0 a 10)
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dis(0.0, 10.0);
    for (int i = 0; i < m->n_linhas; i++) {
        for (int j = 0; j < m->n_colunas; j++) {
            m->dados[i][j] = dis(gen);
        }
    }
}

bool salvar_matriz_arquivo(Matriz* m, const std::string& nome_arquivo) {
    std::ofstream arq_saida(nome_arquivo);
    if (!arq_saida.is_open()) {
        std::cerr << "Erro ao abrir arquivo para salvar a matriz: " << nome_arquivo << std::endl;
        return false;
    }
    arq_saida << m->n_linhas << " " << m->n_colunas << "\n";
    arq_saida << std::fixed << std::setprecision(4);

    for (int i = 0; i < m->n_linhas; i++) {
        for (int j = 0; j < m->n_colunas; j++) {
            arq_saida << m->dados[i][j] << " ";
        }
        arq_saida << "\n";
    }
    arq_saida.close();
    return true; 
}