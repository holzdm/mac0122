#include "biblioteca.h"

#include <stdio.h>

/* Extraído de fontes-txt/pilha(grafos).txt. */
void busca_em_profundidade(int grafo[NUM_VERTICES][NUM_VERTICES], int origem,
                            int visitado[NUM_VERTICES], int pai[NUM_VERTICES]) {
    int pilha[NUM_VERTICES];
    int t = 0;
    for (int i = 0; i < NUM_VERTICES; i++) {
        visitado[i] = 0;
        pai[i] = -1;
    }
    if (origem < 0 || origem >= NUM_VERTICES) return;
    visitado[origem] = 1;
    pilha[t++] = origem;
    printf("Ordem de saída da pilha: ");
    while (t) {
        int x = pilha[--t];
        printf("%d ", x);
        for (int y = 0; y < NUM_VERTICES; y++) {
            if (grafo[x][y] && !visitado[y]) {
                visitado[y] = 1;
                pai[y] = x;
                pilha[t++] = y;
            }
        }
    }
    putchar('\n');
}

int main(void) {
    int grafo[NUM_VERTICES][NUM_VERTICES];
    int visitado[NUM_VERTICES], pai[NUM_VERTICES];
    copia_grafo(grafo);
    busca_em_profundidade(grafo, 0, visitado, pai);
    for (int i = 0; i < NUM_VERTICES; i++)
        if (!visitado[i] || (i && (pai[i] < 0 || !grafo[pai[i]][i]))) return 1;
    printf("Caminho até %d (de trás para frente): ", NUM_VERTICES - 1);
    for (int x = NUM_VERTICES - 1; x >= 0; x = pai[x]) {
        printf("%d%s", x, x ? " <- " : "\n");
        if (!x) break;
    }
    return 0;
}
