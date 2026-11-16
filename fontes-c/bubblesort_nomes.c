#include "biblioteca.h"

#include <stdio.h>
#include <string.h>

/* Extraído de fontes-txt/bubblesort(nomes).txt. */
void bubblesort_nomes(const char *nomes[], int n) {
    for (int fim = n; fim > 1; fim--) {
        int trocou = 0;
        for (int i = 0; i < fim - 1; i++) {
            if (strcmp(nomes[i], nomes[i + 1]) > 0) {
                const char *aux = nomes[i];
                nomes[i] = nomes[i + 1];
                nomes[i + 1] = aux;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}

int main(void) {
    const char *nomes[TAM_NOMES];
    for (int i = 0; i < TAM_NOMES; i++) nomes[i] = nomes_exemplo[i];
    bubblesort_nomes(nomes, TAM_NOMES);
    for (int i = 0; i < TAM_NOMES; i++) {
        puts(nomes[i]);
        if (i && strcmp(nomes[i - 1], nomes[i]) > 0) return 1;
    }
    return 0;
}
