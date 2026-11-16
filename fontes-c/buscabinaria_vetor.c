#include "biblioteca.h"

#include <stdio.h>

/* Extraído de fontes-txt/buscabinaria(vetor).txt. */
int busca_binaria(const int v[], int n, int x) {
    int esq = 0;
    int dir = n - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        if (v[meio] == x) return meio;
        if (v[meio] < x) esq = meio + 1;
        else dir = meio - 1;
    }
    return -1;
}

int main(void) {
    int vetor[TAM_VETOR];
    copia_vetor_ordenado(vetor);
    printf("Vetor ordenado: ");
    imprime_vetor(vetor, TAM_VETOR);
    int x = 25;
    int posicao = busca_binaria(vetor, TAM_VETOR, x);
    printf("%d encontrado no índice %d\n", x, posicao);
    return posicao >= 0 && vetor[posicao] == x &&
           busca_binaria(vetor, TAM_VETOR, 99) == -1 ? 0 : 1;
}
