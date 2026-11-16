#include "biblioteca.h"

/* Extraído de fontes-txt/bubblesort.txt. */
void bubblesort(int *vetor, int n) {
    while (n > 1) {
        for (int i = 0; i < n - 1; i++) {
            if (vetor[i] > vetor[i + 1]) {
                int aux = vetor[i];
                vetor[i] = vetor[i + 1];
                vetor[i + 1] = aux;
            }
        }
        n--;
    }
}

int main(void) {
    return demonstra_ordenacao("Bubble Sort", bubblesort);
}
