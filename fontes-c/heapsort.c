#include "biblioteca.h"

/* Extraído de fontes-txt/heapsort.txt; índices adaptados para começar em zero. */
static void troca(int *a, int *b) {
    int aux = *a;
    *a = *b;
    *b = aux;
}

static void peneira(int v[], int raiz, int tamanho) {
    while (2 * raiz + 1 < tamanho) {
        int filho = 2 * raiz + 1;
        if (filho + 1 < tamanho && v[filho] < v[filho + 1]) filho++;
        if (v[raiz] >= v[filho]) break;
        troca(&v[raiz], &v[filho]);
        raiz = filho;
    }
}

void heapsort(int v[], int n) {
    for (int i = n / 2 - 1; i >= 0; i--) peneira(v, i, n);
    for (int fim = n - 1; fim > 0; fim--) {
        troca(&v[0], &v[fim]);
        peneira(v, 0, fim);
    }
}

int main(void) {
    return demonstra_ordenacao("Heap Sort", heapsort);
}
