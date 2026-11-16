#include "biblioteca.h"

#include <stdio.h>
#include <string.h>

/* Extraído de fontes-txt/frequencia(nomes).txt. */
int frequencia_nomes(const char *nomes[], int n) {
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
    int total = 0;
    for (int i = 0; i < n;) {
        int j = i + 1;
        while (j < n && strcmp(nomes[i], nomes[j]) == 0) j++;
        printf("%s: %d\n", nomes[i], j - i);
        total += j - i;
        i = j;
    }
    return total;
}

int main(void) {
    const char *nomes[TAM_NOMES];
    for (int i = 0; i < TAM_NOMES; i++) nomes[i] = nomes_exemplo[i];
    return frequencia_nomes(nomes, TAM_NOMES) == TAM_NOMES ? 0 : 1;
}
