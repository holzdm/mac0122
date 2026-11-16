#include "biblioteca.h"

#include <stdio.h>

typedef struct {
    int pilha[TAM_VETOR];
    int t;
} PilhaVetor;

/* Extraído de fontes-txt/pilha(vetor).txt: pilha[t++] = y; pilha[--t]. */
int empilha(PilhaVetor *p, int y) {
    if (p->t == TAM_VETOR) return 0;
    p->pilha[p->t++] = y;
    return 1;
}

int desempilha(PilhaVetor *p, int *x) {
    if (p->t == 0) return 0;
    *x = p->pilha[--p->t];
    return 1;
}

int main(void) {
    int dados[TAM_VETOR];
    copia_vetor(dados);
    PilhaVetor p = { .t = 0 };
    for (int i = 0; i < TAM_VETOR; i++)
        if (!empilha(&p, dados[i])) return 1;
    if (empilha(&p, 99)) return 1;
    printf("Saída LIFO: ");
    for (int i = TAM_VETOR - 1; i >= 0; i--) {
        int x;
        if (!desempilha(&p, &x) || x != dados[i]) return 1;
        printf("%s%d", i == TAM_VETOR - 1 ? "" : " ", x);
    }
    putchar('\n');
    int x;
    return desempilha(&p, &x) ? 1 : 0;
}
