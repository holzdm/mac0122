#include "biblioteca.h"

#include <stdio.h>

/* Extraído de fontes-txt/buscalargura(grafos).txt. */
void busca_largura(int grafo[NUM_VERTICES][NUM_VERTICES], int origem,
                   int dist[NUM_VERTICES], int pai[NUM_VERTICES]) {
    int fila[NUM_VERTICES];
    int inicio = 0, fim = 0;
    for (int x = 0; x < NUM_VERTICES; x++) {
        dist[x] = -1;
        pai[x] = -1;
    }
    if (origem < 0 || origem >= NUM_VERTICES) return;
    dist[origem] = 0;
    fila[fim++] = origem;
    while (inicio < fim) {
        int x = fila[inicio++];
        for (int y = 0; y < NUM_VERTICES; y++) {
            if (grafo[x][y] && dist[y] == -1) {
                dist[y] = dist[x] + 1;
                pai[y] = x;
                fila[fim++] = y;
            }
        }
    }
}

int main(void) {
    int grafo[NUM_VERTICES][NUM_VERTICES];
    int dist[NUM_VERTICES], pai[NUM_VERTICES];
    copia_grafo(grafo);
    busca_largura(grafo, 0, dist, pai);
    for (int i = 0; i < NUM_VERTICES; i++) {
        printf("Vértice %d: distância %d, pai %d\n", i, dist[i], pai[i]);
        if (dist[i] < 0 || (i != 0 &&
            (pai[i] < 0 || !grafo[pai[i]][i] || dist[pai[i]] + 1 != dist[i]))) return 1;
    }
    return 0;
}
