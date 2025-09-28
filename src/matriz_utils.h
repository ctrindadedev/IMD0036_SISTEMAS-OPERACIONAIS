#ifndef MATRIZ_UTILS_H
#define MATRIZ_UTILS_H

#include <vector> 
#include <string>  

struct Matriz {
    int n_linhas;
    int n_colunas;
    std::vector<std::vector<double>> dados;
};

//Vetor
Matriz* criar_matriz(int linhas, int colunas);


void liberar_matriz(Matriz* m);


void preencher_matriz(Matriz* m);


bool salvar_matriz_arquivo(Matriz* m, const std::string& nome_arquivo);

#endif 