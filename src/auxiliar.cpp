#include <iostream>  
#include <string>    
#include <cstdlib>   
#include "matriz_utils.h" 

int main(int argc, char* argv[]) {    
    if (argc != 5) {
        std::cerr << "Uso: " << argv[0] << " [n1] [m1] [n2] [m2], m1 precisa ser igual a n2." << std::endl;
        return 1;
    }

    // Converter a entrada em inputs do tipo INT
    int n1 = std::atoi(argv[1]);
    int m1 = std::atoi(argv[2]);
    int n2 = std::atoi(argv[3]);
    int m2 = std::atoi(argv[4]);

    if (n1 <= 0 || m1 <= 0 || n2 <= 0 || m2 <= 0) {
        std::cerr << "Programa encerrando: Dimensoes devem ser numeros positivos" << std::endl;
        return 1;
    }

    if (m1 != n2) {
        std::cerr << "Programa encerrando: O número de colunas (m1) de M1, deve ser igual ao número delinhas (n2) de M2 " << std::endl;
        return 1; 
    }    

    std::cout << "Gerando matriz M1 (" << n1 << "x" << m1 << ") e M2 (" << n2 << "x" << m2 << ")" << std::endl;
    

    std::string file_m1 = "data/m1.txt";
    Matriz* mat1 = criar_matriz(n1, m1);
    if (mat1 == nullptr) {
        std::cerr << "Erro ao criar a M1" << std::endl;
        return 1;
    }
    
     preencher_matriz(mat1);
    
    if (!salvar_matriz_arquivo(mat1, file_m1)) {
        std::cerr << "Erro ao salvar M1 em " << file_m1 << std::endl;
        liberar_matriz(mat1);
        return 1;
    }
    std::cout << "Matriz M1 salva em " << file_m1 << std::endl;
    liberar_matriz(mat1); 

 
    std::string file_m2 = "data/m2.txt";
    Matriz* mat2 = criar_matriz(n2, m2);
    if (mat2 == nullptr) {
        std::cerr << "Erro ao criar M1" << std::endl;
        return 1;
    }

    preencher_matriz(mat2);

    if (!salvar_matriz_arquivo(mat2, file_m2)) {
        std::cerr << "Erro ao salvar M2 em " << file_m2 << std::endl;
        liberar_matriz(mat2);
        return 1;
    }

    std::cout << "Matriz M2 salva em " << file_m2 << " , Programa concluido" <<std::endl;
    liberar_matriz(mat2);

}