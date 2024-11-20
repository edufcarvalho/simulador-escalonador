# Simulador de Escalonamento de Processos - Round Robin com Feedback

### 📘 Descrição
Este projeto implementa um simulador de escalonamento de processos que utiliza o algoritmo Round Robin com Feedback, escrito em C. O simulador é parte de um trabalho para a disciplina de Sistemas Operacionais da UFRJ, cujo objetivo é auxiliar no entendimento do gerenciamento de processos e escalonamento em sistemas operacionais.

### 🎯 Objetivo
Simular o comportamento de processos em um ambiente de escalonamento com:
- Filas de prioridade e gerenciamento de I/O.
- Estratégia Round Robin com Feedback para distribuição justa do tempo de CPU entre processos.

### 🚀 Funcionalidades
- **Filas de Prioridade:** Processos são distribuídos em filas de alta e baixa prioridade, e uma fila de I/O com dispositivos diferentes.
- **I/O Simulado:** Processos utilizam I/O (disco, fita magnética e impressora) e retornam a filas específicas de acordo com o dispositivo utilizado.
- **Fatia de Tempo (Time Slice):** Implementa uma fatia de tempo definida pelo usuário para execução dos processos.
- **Simulação do Algoritmo:** Implementa a movimentação dos processos nas filas, com preempção e feedback para controlar a prioridade.

### 📂 Estrutura do Projeto

    ├── docs/                    # Arquivos adicionais
    │   └── relatorio.pdf        # Relatório do projeto
    ├── src/                     # Código-fonte do simulador
    │   ├── main.c               # Arquivo principal do simulador que contém todas as funções
    ├── Makefile                 # Lógica de compilação
    └── README.md                # Descrição do projeto

### ⚙️ Requisitos
- **Compilador C:** gcc (ou outro compatível)
- **Makefile:** Precisa estar instalado

### 🛠️ Instalação e Execução

1. **Clone o repositório:**
  ```git clone https://github.com/edufcarvalho/round-robin-feedback.git```

2. **Troque para a pasta correta:**
  ```cd round-robin-feedback```

3. **Compile o código:**
  ```make```

4. **Execute o simulador:**
  ```make run```

### 📊 Exemplo de Saída
Abaixo está um exemplo de saída gerada pelo simulador. Ele mostra o comportamento dos processos nas filas.

```plaintext
TEMPO = 9 

Fila Finalizados:
Fila Alta Prioridade:
Fila Baixa Prioridade:
[  1  ] [  4  ] [  3  ]
Fila Disco:
Fila Fita:
[  2  ]
Fila Impressora:
Fila Bloqueados:
[  2  ]
```

### 👥 Autores
- [Bruna Ribeiro](https://github.com/brunapr)
- [Eduardo Freitas de Carvalho](https://github.com/edufcarvalho)
- [Lucas Fernandes Silva](https://github.com/fslucasfs)

