#include "biblioteca.h"

#include <stdio.h>

/* Extraído de fontes-txt/buscaprofundidade(grafos).txt. */
static void dfs(int grafo[NUM_VERTICES][NUM_VERTICES], int x,
                int visitado[NUM_VERTICES], int pai[NUM_VERTICES]) {
    visitado[x] = 1;
    printf("%d ", x);
    for (int y = 0; y < NUM_VERTICES; y++) {
        if (grafo[x][y] && !visitado[y]) {
            pai[y] = x;
            dfs(grafo, y, visitado, pai);
        }
    }
}

void busca_profundidade(int grafo[NUM_VERTICES][NUM_VERTICES], int origem,
                         int visitado[NUM_VERTICES], int pai[NUM_VERTICES]) {
    for (int x = 0; x < NUM_VERTICES; x++) {
        visitado[x] = 0;
        pai[x] = -1;
    }
    if (origem >= 0 && origem < NUM_VERTICES) dfs(grafo, origem, visitado, pai);
}

int main(void) {
    int grafo[NUM_VERTICES][NUM_VERTICES];
    int visitado[NUM_VERTICES], pai[NUM_VERTICES];
    copia_grafo(grafo);
    printf("Ordem DFS: ");
    busca_profundidade(grafo, 0, visitado, pai);
    putchar('\n');
    for (int i = 0; i < NUM_VERTICES; i++) {
        if (!visitado[i] || (i != 0 && (pai[i] < 0 || !grafo[pai[i]][i]))) return 1;
    }
    return 0;
}
