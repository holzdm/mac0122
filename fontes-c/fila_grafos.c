#include "biblioteca.h"

#include <stdio.h>

/* Extraído de fontes-txt/fila(grafos).txt. */
void busca_em_largura(int grafo[NUM_VERTICES][NUM_VERTICES], int origem,
                      int dist[NUM_VERTICES], int pai[NUM_VERTICES]) {
    int fila[NUM_VERTICES];
    int p = 0, u = 0;
    for (int i = 0; i < NUM_VERTICES; i++) {
        dist[i] = -1;
        pai[i] = -1;
    }
    if (origem < 0 || origem >= NUM_VERTICES) return;
    dist[origem] = 0;
    fila[u++] = origem;
    while (p != u) {
        int x = fila[p++];
        for (int y = 0; y < NUM_VERTICES; y++) {
            if (grafo[x][y] && dist[y] == -1) {
                dist[y] = dist[x] + 1;
                pai[y] = x;
                fila[u++] = y;
            }
        }
    }
}

int imprime_caminho(int origem, int destino, const int dist[NUM_VERTICES],
                    const int pai[NUM_VERTICES]) {
    if (destino < 0 || destino >= NUM_VERTICES || dist[destino] == -1) return 0;
    int caminho[NUM_VERTICES];
    int t = 0;
    for (int atual = destino; atual != origem; atual = pai[atual]) {
        if (atual < 0 || t >= NUM_VERTICES) return 0;
        caminho[t++] = atual;
    }
    caminho[t++] = origem;
    printf("Menor caminho de %d até %d (%d arestas): ", origem, destino, dist[destino]);
    while (t) {
        t--;
        printf("%d%s", caminho[t], t ? " -> " : "\n");
    }
    return 1;
}

int main(void) {
    int grafo[NUM_VERTICES][NUM_VERTICES];
    int dist[NUM_VERTICES], pai[NUM_VERTICES];
    copia_grafo(grafo);
    busca_em_largura(grafo, 0, dist, pai);
    for (int i = 0; i < NUM_VERTICES; i++)
        if (dist[i] < 0 || (i && (pai[i] < 0 || !grafo[pai[i]][i]))) return 1;
    return imprime_caminho(0, NUM_VERTICES - 1, dist, pai) ? 0 : 1;
}
