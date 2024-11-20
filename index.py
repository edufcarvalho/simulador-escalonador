from dataclasses import dataclass
from typing import List
import time

FINALIZADOS      = 0
ALTA_PRIORIDADE  = 1
BAIXA_PRIORIDADE = 2
DISCO            = 3
FITA             = 4
IMPRESSORA       = 5


PID = 0

fila_alta_prioridade = []
fila_baixa_prioridade = []
# IO
fila_disco = []
fila_fita = []
fila_impressora = []
#bloqueados
fila_bloqueados = []
# Quando acaba
finalizados = []

historico_disco = []
historico_fita = []
historico_impressora = []

historico_alta_prioridade = []
historico_baixa_prioridade = []

historico_uso_cpu = []

@dataclass
class IO:
    chegada: int
    tipo: int
    tempo_utilizado: int
    PID: int


@dataclass
class Processo:
    PID: int
    chegada: int
    tempo_total: int
    tempo_utilizado: int
    io: List[IO]

def ler_processos_arquivo(caminho):
    conteudo = ""
    with open(caminho, 'r') as arquivo:
        conteudo = arquivo.read()

    processos = []
    for linha in conteudo.split("\n"):
        if linha.strip():  # Ensure the line is not empty.
            numeros = list(map(int, linha.split()))
            primeira_posicao = numeros[0]
            segunda_posicao = numeros[1]
            pares = [[numeros[i], numeros[i + 1]] for i in range(2, len(numeros), 2)]
            processos.append([primeira_posicao, segunda_posicao, pares])
    
    return processos

def executar_processo():
    processo_disco = None
    processo_fita = None
    processo_impressora = None

    if len(fila_disco) > 0:
        processo_disco = executar_fila_disco()
    if len(fila_fita) > 0:
        processo_fita = executar_fila_fita()
    if len(fila_impressora) > 0:
        processo_impressora = executar_fila_impressora()

    if len(fila_alta_prioridade) > 0 and fila_alta_prioridade[0] not in [processo_disco, processo_fita, processo_impressora]:
        executar_fila_alta_prioridade()
    elif len(fila_baixa_prioridade) > 0 and fila_baixa_prioridade[0] not in [processo_disco, processo_fita, processo_impressora]:
        executar_fila_baixa_prioridade()
    

def executar_fila_alta_prioridade():
    processo = fila_alta_prioridade[0]
    historico_alta_prioridade.append(processo)
    historico_uso_cpu.append(processo)
    processo.tempo_utilizado += 1
    if len(processo.io) > 0 and processo.tempo_utilizado >= processo.io[0].chegada:
        if processo.io[0].tipo == DISCO:
            fila_disco.append(processo.io[0])
        if processo.io[0].tipo == FITA:
            fila_fita.append(processo.io[0])
        if processo.io[0].tipo == IMPRESSORA:
            fila_impressora.append(processo.io[0])

        del fila_alta_prioridade[0].io[0]
        fila_bloqueados.append(processo)
        del fila_alta_prioridade[0]
        return;
    
    if processo.tempo_total <= processo.tempo_utilizado:
        finalizados.append(processo)
        del fila_alta_prioridade[0]
        return;

    fila_baixa_prioridade.append(processo)
    del fila_alta_prioridade[0]
    return;
    

def executar_fila_baixa_prioridade():
    processo = fila_baixa_prioridade[0]
    processo.tempo_utilizado += 1
    historico_baixa_prioridade.append(processo)
    historico_uso_cpu.append(processo)
    if len(processo.io) > 0 and processo.tempo_utilizado >= processo.io[0].chegada:
        if processo.io[0].tipo == DISCO:
            fila_disco.append(processo.io[0])
        if processo.io[0].tipo == FITA:
            fila_fita.append(processo.io[0])
        if processo.io[0].tipo == IMPRESSORA:
            fila_impressora.append(processo.io[0])

        del fila_baixa_prioridade[0].io[0]
        fila_bloqueados.append(processo)
        del fila_baixa_prioridade[0]
        return;
    
    if processo.tempo_total <= processo.tempo_utilizado:
        finalizados.append(processo)
        del fila_baixa_prioridade[0]
        return;

    fila_baixa_prioridade.append(processo)
    del fila_baixa_prioridade[0]
    return;

def procurar_processo_bloqueado(PID):
    for i in range(len(fila_bloqueados)):
        if fila_bloqueados[i].PID == PID:
            tmp = fila_bloqueados[i]
            del fila_bloqueados[i]
            return tmp
    
    return None
            
def executar_fila_disco():
    processo = fila_disco[0]
    processo.tempo_utilizado += 1
    print(processo)
    historico_disco.append(processo)
    if processo.tempo_utilizado >= DISCO:
        processo_bloqueado = procurar_processo_bloqueado(processo.PID)
        del fila_disco[0]
        fila_baixa_prioridade.append(processo_bloqueado)
        return processo_bloqueado
    
    return

def executar_fila_fita():
    processo = fila_fita[0]
    processo.tempo_utilizado += 1
    historico_fita.append(processo)
    if processo.tempo_utilizado >= FITA:
        processo_bloqueado = procurar_processo_bloqueado(processo.PID)
        del fila_fita[0]
        fila_alta_prioridade.append(processo_bloqueado)
        return processo_bloqueado
    
    return

def executar_fila_impressora():
    processo = fila_impressora[0]
    processo.tempo_utilizado += 1
    historico_impressora.append(processo)
    if processo.tempo_utilizado >= IMPRESSORA:
        processo_bloqueado = procurar_processo_bloqueado(processo.PID)
        del fila_impressora[0]
        fila_alta_prioridade.append(processo_bloqueado)
        return processo_bloqueado
    
    return


def adicionar_novo_processo_fila(processo: Processo):
    global PID, fila_alta_prioridade
    PID += 1
    ios = [IO(chegada=io[0], tipo=io[1], tempo_utilizado=0, PID=PID) for io in processo[2]]
    novo_processo = Processo(PID=PID, chegada=processo[0], tempo_total=processo[1], tempo_utilizado=0, io=ios)
    fila_alta_prioridade = [novo_processo] + fila_alta_prioridade

def ordernar_por_tempo_chegada(tabela: List[Processo]):
    for i in range(len(tabela)):
        for j in range(len(tabela)):
            if tabela[i][0] < tabela[j][0]:
                tmp = tabela[i]
                tabela[i] = tabela[j]
                tabela[j] = tmp
    
    return tabela

def entrada_processos(tabela: List[Processo], tempo_atual: int):
    tabela = ordernar_por_tempo_chegada(tabela)
    for i in range(len(tabela)):
        if tabela[i][0] <= tempo_atual:
            adicionar_novo_processo_fila(tabela[i])
            del tabela[i]
            break

tabela_processos = ler_processos_arquivo("processos.txt")
numero_processos = len(tabela_processos)
tempo_atual = 0

def print_pids(title, lista):
    print(f"{title}:")
    print(" ".join(f"[{p.PID:^5}]" for p in lista))


while True:
    entrada_processos(tabela_processos, tempo_atual)
    print(f"\nTEMPO = {tempo_atual} \n")

    print_pids("Fila Finalizados", finalizados)
    print_pids("Fila Alta Prioridade", fila_alta_prioridade)
    print_pids("Fila Baixa Prioridade", fila_baixa_prioridade)
    print_pids("Fila Disco", fila_disco)
    print_pids("Fila Fita", fila_fita)
    print_pids("Fila Impressora", fila_impressora)
    print_pids("Fila Bloqueados", fila_bloqueados)

    if len(finalizados) == numero_processos or tempo_atual > 200:
        break

    executar_processo()
    # print_pids("Histórico Disco", historico_disco)
    # print_pids("Histórico Fita", historico_fita)
    # print_pids("Histórico Impressora", historico_impressora)
    # print_pids("Histórico Alta Prioridade", historico_alta_prioridade)
    # print_pids("Histórico Baixa Prioridade", historico_baixa_prioridade)
    # print_pids("Histórico Uso CPU", historico_uso_cpu)

    print("\n\n")

    tempo_atual += 1
    time.sleep(2)
