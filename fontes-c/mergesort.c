#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>

/* Extraído de fontes-txt/mergesort.txt. Os limites são inclusivos. */
static void intercala(int *vetor, int inicio, int meio, int fim, int *aux) {
    int i = inicio;
    int j = meio + 1;
    int k = inicio;
    while (i <= meio && j <= fim)
        aux[k++] = vetor[i] <= vetor[j] ? vetor[i++] : vetor[j++];
    while (i <= meio) aux[k++] = vetor[i++];
    while (j <= fim) aux[k++] = vetor[j++];
    for (k = inicio; k <= fim; k++) vetor[k] = aux[k];
}

static void ordena_intervalo(int *vetor, int inicio, int fim, int *aux) {
    if (inicio >= fim) return;
    int meio = inicio + (fim - inicio) / 2;
    ordena_intervalo(vetor, inicio, meio, aux);
    ordena_intervalo(vetor, meio + 1, fim, aux);
    intercala(vetor, inicio, meio, fim, aux);
}

void mergesort(int *vetor, int n) {
    if (n < 2) return;
    int *aux = malloc((size_t)n * sizeof *aux);
    if (!aux) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    ordena_intervalo(vetor, 0, n - 1, aux);
    free(aux);
}

int main(void) {
    return demonstra_ordenacao("Merge Sort", mergesort);
}
