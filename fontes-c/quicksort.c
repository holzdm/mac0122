#include "biblioteca.h"

/* Extraído de fontes-txt/quicksort.txt. */
static void ordena_intervalo(int *vetor, int inicio, int fim) {
    if (inicio >= fim) return;
    int pivo = vetor[fim];
    int maioresq = inicio;
    int menordir = fim - 1;
    while (maioresq <= menordir) {
        while (maioresq <= menordir && vetor[maioresq] <= pivo) maioresq++;
        while (maioresq <= menordir && vetor[menordir] >= pivo) menordir--;
        if (maioresq < menordir) {
            int aux = vetor[maioresq];
            vetor[maioresq] = vetor[menordir];
            vetor[menordir] = aux;
        }
    }
    vetor[fim] = vetor[maioresq];
    vetor[maioresq] = pivo;
    ordena_intervalo(vetor, inicio, maioresq - 1);
    ordena_intervalo(vetor, maioresq + 1, fim);
}

void quicksort(int *vetor, int n) {
    if (n > 1) ordena_intervalo(vetor, 0, n - 1);
}

int main(void) {
    return demonstra_ordenacao("Quick Sort", quicksort);
}
