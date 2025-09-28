# Makefile para projeto de multiplicação de matrizes
# Compilador
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2

# Bibliotecas necessárias
LDFLAGS = -pthread

# Diretórios
SRC_DIR = .
BIN_DIR = ../bin
DATA_DIR = ../data
RESULTS_DIR = ../results

# Arquivos fonte
AUXILIAR_SRC = auxiliar.cpp matriz_utils.cpp
SEQUENCIAL_SRC = algoritmo_sequencial.cpp matriz_utils.cpp
PARALELO_PROCESS_SRC = algoritmo_paralelo_process.cpp matriz_utils.cpp
PARALELO_THREADS_SRC = algoritmo_paralelo_threads.cpp matriz_utils.cpp

# Executáveis
AUXILIAR = $(BIN_DIR)/auxiliar
SEQUENCIAL = $(BIN_DIR)/algoritmo_sequencial
PARALELO_PROCESS = $(BIN_DIR)/algoritmo_paralelo_process
PARALELO_THREADS = $(BIN_DIR)/algoritmo_paralelo_threads

# Regra padrão
all: $(AUXILIAR) $(SEQUENCIAL) $(PARALELO_PROCESS) $(PARALELO_THREADS)

# Criar diretórios necessários
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(DATA_DIR):
	mkdir -p $(DATA_DIR)

$(RESULTS_DIR):
	mkdir -p $(RESULTS_DIR)

# Compilar gerador de matrizes
$(AUXILIAR): $(AUXILIAR_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(AUXILIAR_SRC)

# Compilar algoritmo sequencial
$(SEQUENCIAL): $(SEQUENCIAL_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(SEQUENCIAL_SRC)

# Compilar algoritmo paralelo com processos
$(PARALELO_PROCESS): $(PARALELO_PROCESS_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $(PARALELO_PROCESS_SRC)

# Compilar algoritmo paralelo com threads
$(PARALELO_THREADS): $(PARALELO_THREADS_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(PARALELO_THREADS_SRC)

# Gerar matrizes de teste
test-data: $(AUXILIAR) | $(DATA_DIR)
	./$(AUXILIAR) 100 100 100 100

# Executar testes
test-sequential: $(SEQUENCIAL) | $(RESULTS_DIR)
	./$(SEQUENCIAL) $(DATA_DIR)/m1.txt $(DATA_DIR)/m2.txt

test-process: $(PARALELO_PROCESS) | $(RESULTS_DIR)
	./$(PARALELO_PROCESS) $(DATA_DIR)/m1.txt $(DATA_DIR)/m2.txt 4

test-threads: $(PARALELO_THREADS) | $(RESULTS_DIR)
	./$(PARALELO_THREADS) $(DATA_DIR)/m1.txt $(DATA_DIR)/m2.txt 4

# Executar todos os testes
test: test-data test-sequential test-process test-threads

# Limpar executáveis
clean:
	rm -f $(BIN_DIR)/*

# Limpar resultados
clean-results:
	rm -f $(RESULTS_DIR)/*

# Limpar dados
clean-data:
	rm -f $(DATA_DIR)/*

# Limpar tudo
clean-all: clean clean-results clean-data

# Ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make all          - Compila todos os executáveis"
	@echo "  make test-data     - Gera matrizes de teste"
	@echo "  make test-sequential - Executa algoritmo sequencial"
	@echo "  make test-process  - Executa algoritmo paralelo com processos"
	@echo "  make test-threads  - Executa algoritmo paralelo com threads"
	@echo "  make test          - Executa todos os testes"
	@echo "  make clean         - Remove executáveis"
	@echo "  make clean-results - Remove resultados"
	@echo "  make clean-data    - Remove dados de teste"
	@echo "  make clean-all     - Remove tudo"
	@echo "  make help          - Mostra esta ajuda"

.PHONY: all test-data test-sequential test-process test-threads test clean clean-results clean-data clean-all help
