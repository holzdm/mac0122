#include "biblioteca.h"

#include <stdio.h>

/* Extraído de fontes-txt/hanoi.txt. */
unsigned long long hanoi(int n, int origem, int destino, int auxiliar) {
    if (n == 0) return 0;
    unsigned long long movimentos = hanoi(n - 1, origem, auxiliar, destino);
    printf("%d -> %d\n", origem, destino);
    movimentos++;
    movimentos += hanoi(n - 1, auxiliar, destino, origem);
    return movimentos;
}

int main(void) {
    const int n = 4;
    unsigned long long total_jogadas = hanoi(n, 1, 3, 2);
    printf("Total de jogadas: %llu\n", total_jogadas);
    return total_jogadas == (1ULL << n) - 1 ? 0 : 1;
}
