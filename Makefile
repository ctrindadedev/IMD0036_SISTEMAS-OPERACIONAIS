# Makefile para projeto de multiplicação de matrizes
# Compilador
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2 -g

# Flags de linkagem (para bibliotecas)
LDFLAGS = -pthread
SRC_DIR = src
BIN_DIR = bin
DATA_DIR = data
RESULTS_DIR = results



# Arquivos  Fonte e Objeto ecessários para cada executável

SRCS = $(wildcard $(SRC_DIR)/*.cpp) # Pega todos os arquivos .cpp da pasta src
OBJS = $(SRCS:.cpp=.o)


# Executáveis
AUXILIAR_EXEC = $(BIN_DIR)/auxiliar
SEQUENCIAL_EXEC = $(BIN_DIR)/algoritmo_sequencial
PARALELO_PROCESS_EXEC = $(BIN_DIR)/algoritmo_paralelo_process
PARALELO_THREADS_EXEC = $(BIN_DIR)/algoritmo_paralelo_threads

# Regra padrão
all: $(AUXILIAR_EXEC) $(SEQUENCIAL_EXEC) $(PARALELO_PROCESS_EXEC) $(PARALELO_THREADS_EXEC)

# Regras de Linkagem - Criação dos executáveis a partir dos .o

$(AUXILIAR_EXEC): $(SRC_DIR)/auxiliar.o $(SRC_DIR)/matriz_utils.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(SEQUENCIAL_EXEC): $(SRC_DIR)/algoritmo_sequencial.o $(SRC_DIR)/matriz_utils.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(PARALELO_PROCESS_EXEC): $(SRC_DIR)/algoritmo_paralelo_process.o $(SRC_DIR)/matriz_utils.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(PARALELO_THREADS_EXEC): $(SRC_DIR)/algoritmo_paralelo_threads.o $(SRC_DIR)/matriz_utils.o | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Regras de Compilação -Criação dos .o a partir dos .cpp

$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(SRC_DIR)/matriz_utils.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regras de Utilitários e Testes (Facilitar mudanças no algoritmo)

# Criar diretórios necessários 
$(BIN_DIR) $(DATA_DIR) $(RESULTS_DIR):
	@mkdir -p $@

# Gerar matrizes de teste
test-data: $(AUXILIAR_EXEC) | $(DATA_DIR)
	@echo "Gerando matrizes de teste 100x100"
	@$(AUXILIAR_EXEC) 100 100 100 100

# Executar testes 
test-sequential: $(SEQUENCIAL_EXEC) | $(RESULTS_DIR)
	@echo "Testando Algoritmo Sequencial"
	@$(SEQUENCIAL_EXEC) $(DATA_DIR)/m1.txt $(DATA_DIR)/m2.txt

test-process: $(PARALELO_PROCESS_EXEC) | $(RESULTS_DIR)
	@echo "Testando Algoritmo Paralelo (Processos)"
	@$(PARALELO_PROCESS_EXEC) $(DATA_DIR)/m1.txt $(DATA_DIR)/m2.txt 4000

test-threads: $(PARALELO_THREADS_EXEC) | $(RESULTS_DIR)
	@echo "Testando Algoritmo Paralelo (Threads)"
	@$(PARALELO_THREADS_EXEC) $(DATA_DIR)/m1.txt $(DATA_DIR)/m2.txt 4000

# Executar todos os testes
test: test-data test-sequential test-process test-threads

# Regras de Limpeza
clean:
	@echo "Limpando executáveis e arquivos objeto..."
	@rm -f $(BIN_DIR)/* $(SRC_DIR)/*.o

clean-results:
	@echo "Limpando resultados..."
	@rm -f $(RESULTS_DIR)/*

clean-data:
	@echo "Limpando dados de teste..."
	@rm -f $(DATA_DIR)/*

clean-all: clean clean-results clean-data

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make all              - Compila todos os executáveis"
	@echo "  make test-data        - Gera matrizes de teste (100x100)"
	@echo "  make test-sequential  - Executa algoritmo sequencial"
	@echo "  make test-process     - Executa algoritmo paralelo com processos"
	@echo "  make test-threads     - Executa algoritmo paralelo com threads"
	@echo "  make test             - Executa todos os testes"
	@echo "  make clean            - Remove executáveis e arquivos .o"
	@echo "  make clean-results    - Remove resultados"
	@echo "  make clean-data       - Remove dados de teste"
	@echo "  make clean-all        - Remove tudo"
	@echo "  make help             - Mostra esta ajuda"

.PHONY: all test-data test-sequential test-process test-threads test clean clean-results clean-data clean-all help