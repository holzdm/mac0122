#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>

/* Extraído de fontes-txt/kruskal.txt; union-find por rótulos de família. */
void inicializa(int familia[], int tamanho[], int n) {
    for (int i = 0; i < n; i++) {
        familia[i] = i;
        tamanho[i] = 1;
    }
}

void union_f(int familia[], int tamanho[], int n, int A, int B) {
    if (A == B) return;
    int maior = tamanho[A] >= tamanho[B] ? A : B;
    int menor = maior == A ? B : A;
    for (int i = 0; i < n; i++)
        if (familia[i] == menor) familia[i] = maior;
    tamanho[maior] += tamanho[menor];
    tamanho[menor] = 0;
}

static int comparar_arestas(const void *a, const void *b) {
    int x = ((const Aresta *)a)->custo;
    int y = ((const Aresta *)b)->custo;
    return (x > y) - (x < y);
}

int kruskal(int n, int m, Aresta arestas[], Aresta arv_min[]) {
    int familia[NUM_VERTICES], tamanho[NUM_VERTICES];
    if (n < 1 || n > NUM_VERTICES || m < 0) return -1;
    qsort(arestas, (size_t)m, sizeof arestas[0], comparar_arestas);
    inicializa(familia, tamanho, n);
    int adicionadas = 0;
    int custo_total = 0;
    for (int i = 0; i < m && adicionadas < n - 1; i++) {
        int v = arestas[i].v;
        int w = arestas[i].w;
        if (v < 0 || v >= n || w < 0 || w >= n) return -1;
        if (familia[v] != familia[w]) {
            arv_min[adicionadas++] = arestas[i];
            custo_total += arestas[i].custo;
            union_f(familia, tamanho, n, familia[v], familia[w]);
        }
    }
    if (adicionadas != n - 1) return -1;
    return custo_total;
}

int main(void) {
    Aresta arestas[NUM_ARESTAS];
    Aresta arv_min[NUM_VERTICES - 1];
    for (int i = 0; i < NUM_ARESTAS; i++) arestas[i] = arestas_exemplo[i];
    int custo = kruskal(NUM_VERTICES, NUM_ARESTAS, arestas, arv_min);
    if (custo < 0) {
        fputs("Grafo inválido ou desconectado.\n", stderr);
        return 1;
    }
    for (int i = 0; i < NUM_VERTICES - 1; i++)
        printf("(%d, %d) - custo: %d\n", arv_min[i].v, arv_min[i].w, arv_min[i].custo);
    printf("Custo total mínimo: %d\n", custo);
    return custo == 23 ? 0 : 1;
}
