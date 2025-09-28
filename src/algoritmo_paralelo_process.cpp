#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
//específicos para Processos e IPC
#include <unistd.h>    
#include <sys/wait.h>   
#include <sys/ipc.h>    
#include <sys/shm.h>     
#include "matriz_utils.h" 

// É importante chamar essa função para não deixar lixo no SO (Independente de erro ou sucesso)
void cleanup_shm(int shmid) {
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_matriz1> <arquivo_matriz2> <P>" << std::endl;
        return 1;
    }
    std::string arquivo_a = argv[1];
    std::string arquivo_b = argv[2];
    int p = std::atoi(argv[3]);
    Matriz* a = ler_matriz_arquivo(arquivo_a);
    Matriz* b = ler_matriz_arquivo(arquivo_b);

    if (a == nullptr || b == nullptr) {
        return 1;
    }
    if (a->n_colunas != b->n_linhas) {
        std::cerr << "Erro: Matrizes nao multiplicaveis." << std::endl;
        liberar_matriz(a);
        liberar_matriz(b);
        return 1;
    }

    int resultado_linhas = a->n_linhas;
    int resultado_colunas = b->n_colunas;
    long long total_elementos = (long long)resultado_linhas * resultado_colunas;
    int num_processos = std::ceil((double)total_elementos / p);

    std::cout << "Executando com " << num_processos << " processos..." << std::endl;

    // Cria o segmento de memória compartilhada, com ftok para gerar uma chave para a memória
    key_t key = ftok("shmfile", 65);
    if (key == -1) {
        perror("ftok");
        return 1;
    }

    // Tamanho necessário para a matriz resultado
    size_t shm_size = resultado_linhas * resultado_colunas * sizeof(double);

    int shmid = shmget(key, shm_size, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

   //Anexa o segmento de memória ao espaço de endereçamento deste processo

    double* matriz_resultado_shm = (double*)shmat(shmid, NULL, 0);
    if (matriz_resultado_shm == (void*)-1) {
        perror("shmat (pai)");
        cleanup_shm(shmid); 
        return 1;
    }
    
    std::vector<pid_t> pids;  //(PIDs dos filhos)

    for (int i = 0; i < num_processos; ++i) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            cleanup_shm(shmid);
            return 1;
        }

        if (pid == 0) { 
            
            // O filho também precisa se anexar para poder acessar a mesma memória
            double* resultado_filho_ptr = (double*)shmat(shmid, NULL, 0);
            if (resultado_filho_ptr == (void*)-1) {
                perror("shmat (filho)");
                exit(1); 
            }
            
            long long inicio_elemento = (long long)i * p;
            long long fim_elemento = std::min(inicio_elemento + p, total_elementos);

            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (long long idx = inicio_elemento; idx < fim_elemento; ++idx) {
                int linha = idx / resultado_colunas;
                int col = idx % resultado_colunas;
                double soma = 0.0;
                for (int k = 0; k < a->n_colunas; ++k) {
                    soma += a->dados[linha][k] * b->dados[k][col];
                }
                // Escreve o resultado DIRETAMENTE na memória compartilhada
                resultado_filho_ptr[idx] = soma;
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> tempo_execucao = end_time - start_time;

            std::string nome_arq_saida = "results/proc_" + std::to_string(i) + ".txt";
            std::ofstream arq_saida(nome_arq_saida);
            arq_saida << std::fixed << std::setprecision(5);
            
            for (long long idx = inicio_elemento; idx < fim_elemento; ++idx) {
                 arq_saida << resultado_filho_ptr[idx] << "\n";
            }
            arq_saida << "TEMPO," << tempo_execucao.count() << "\n";
            arq_saida.close();

    
            shmdt(resultado_filho_ptr);
            
            exit(0); 
        } else {
            pids.push_back(pid);
        }
    }

    //Pai aguarda todos processos-filhos terminarem
    for (pid_t pid : pids) {
        waitpid(pid, NULL, 0);
    }
    
    std::cout << "Todos os processos filhos terminaram." << std::endl;
    
    // Desanexar da memória compartilhada
    shmdt(matriz_resultado_shm);
    
    // Marcar o segmento para destruição
    cleanup_shm(shmid);    
    
    liberar_matriz(a);
    liberar_matriz(b);

    return 0;
}