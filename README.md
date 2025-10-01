# Multiplicação de Matrizes: Análise de Desempenho Sequencial vs. Paralelo

Este projeto, desenvolvido para a disciplina de Sistemas Operacionais (IMD0036), implementa e compara o desempenho da multiplicação de matrizes usando três abordagens distintas:

1.  **Sequencial:** Um algoritmo single-thread padrão.
2.  **Paralelo com Threads:** Utiliza a biblioteca POSIX Threads (`pthreads`) para dividir o trabalho entre múltiplas threads dentro de um mesmo processo.
3.  **Paralelo com Processos:** Utiliza `fork()` e comunicação interprocessos (IPC) via memória compartilhada (`shmget`, `shmat`) para dividir o trabalho entre múltiplos processos.

O objetivo é analisar os ganhos de desempenho obtidos com o paralelismo e entender o impacto do overhead de gerenciamento de tarefas pelo sistema operacional.

## Fluxo Básico de Clonagem e Teste

Para compilar e executar o projeto, siga os passos abaixo.

### 1. Pré-requisitos

- Um compilador C++ (g++)
- Ferramenta `make`
- Ambiente Linux ou compatível (como WSL no Windows)

### 2. Clonagem do Repositório

Clone o projeto para a sua máquina local com o seguinte comando:

```bash
git clone [https://github.com/ctrindadedev/IMD0036_SISTEMAS-OPERACIONAIS.git](https://github.com/ctrindadedev/IMD0036_SISTEMAS-OPERACIONAIS.git)
cd IMD0036_SISTEMAS-OPERACIONAIS
```

### 3. **Crie e ative um ambiente virtual:**

    É altamente recomendado usar um ambiente virtual para isolar as dependências do projeto, e não ter problemas com as bibliotecas Python.

    ```bash
    # Criar o ambiente virtual
    python3 -m venv .venv

    # Ativar o ambiente (Linux/macOS/WSL)
    source .venv/bin/activate
    ```

### 4. **Instale as dependências:**

    ```bash
    pip install -r requirements.txt
    ```

    _(Para desativar, execute `deactivate`)_

### 5. Compilação

O projeto utiliza um `Makefile` para automatizar todo o processo de compilação. O `Makefile` está na raiz do projeto e os arquivos-fonte estão na pasta `src/`.

Para compilar todos os executáveis, execute o seguinte comando na raiz do projeto:

```bash
make all
```

Isso irá compilar os quatro programas (`auxiliar`, `algoritmo_sequencial`, `algoritmo_paralelo_threads` e `algoritmo_paralelo_process`) e colocá-los na pasta `bin/`.

### 6. Uso e Testes Básicos

O `Makefile` contém comandos úteis para facilitar os testes durante o desenvolvimento, para acessar a lista com todos eles (E informação de outros comandos), basta executar:

```bash
make help
```

### 7. Executar experimentos
