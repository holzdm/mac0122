#include "biblioteca.h"

/* Extraído de fontes-txt/insertionsort.txt. */
void insertionsort(int *vetor, int n) {
    for (int i = 1; i < n; i++) {
        int key = vetor[i];
        int j = i - 1;
        while (j >= 0 && vetor[j] > key) {
            vetor[j + 1] = vetor[j];
            j--;
        }
        vetor[j + 1] = key;
    }
}

int main(void) {
    return demonstra_ordenacao("Insertion Sort", insertionsort);
}
