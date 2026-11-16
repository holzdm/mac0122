#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>

/* Extraído de fontes-txt/countingsort.txt; R é o maior valor permitido. */
static void ordena_contagem(int *v, int n, int R) {
    int *fp = calloc((size_t)R + 1, sizeof *fp);
    int *aux = malloc((size_t)n * sizeof *aux);
    if (!fp || !aux) {
        free(fp);
        free(aux);
        perror("alocação do Counting Sort");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        if (v[i] < 0 || v[i] > R) {
            fputs("Valor fora do intervalo do Counting Sort.\n", stderr);
            free(fp);
            free(aux);
            exit(EXIT_FAILURE);
        }
        fp[v[i]]++;
    }
    for (int r = 1; r <= R; r++) fp[r] += fp[r - 1];
    for (int i = n - 1; i >= 0; i--) aux[--fp[v[i]]] = v[i];
    for (int i = 0; i < n; i++) v[i] = aux[i];
    free(fp);
    free(aux);
}

void countingsort(int *v, int n) {
    int maior = 0;
    for (int i = 0; i < n; i++) if (v[i] > maior) maior = v[i];
    if (n > 0) ordena_contagem(v, n, maior);
}

int main(void) {
    return demonstra_ordenacao("Counting Sort", countingsort);
}
