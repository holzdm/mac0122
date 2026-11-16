#include "biblioteca.h"

/* Extraído de fontes-txt/selectionsort.txt; a busca inclui o último item. */
void selectionsort(int *v, int n) {
    for (int i = 0; i < n - 1; i++) {
        int min = i;
        for (int j = i + 1; j < n; j++) {
            if (v[j] < v[min]) min = j;
        }
        if (min != i) {
            int aux = v[i];
            v[i] = v[min];
            v[min] = aux;
        }
    }
}

int main(void) {
    return demonstra_ordenacao("Selection Sort", selectionsort);
}
