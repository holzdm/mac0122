#include "biblioteca.h"

#include <stdio.h>

typedef struct {
    int fila[TAM_VETOR];
    int p;
    int u;
    int quantidade;
} FilaVetor;

/* Extraído de fontes-txt/fila(vetor); índices circulares reutilizam posições. */
int enfileira(FilaVetor *f, int y) {
    if (f->quantidade == TAM_VETOR) return 0;
    f->fila[f->u] = y;
    f->u = (f->u + 1) % TAM_VETOR;
    f->quantidade++;
    return 1;
}

int desenfileira(FilaVetor *f, int *x) {
    if (f->quantidade == 0) return 0;
    *x = f->fila[f->p];
    f->p = (f->p + 1) % TAM_VETOR;
    f->quantidade--;
    return 1;
}

int main(void) {
    int dados[TAM_VETOR];
    copia_vetor(dados);
    FilaVetor f = {0};
    for (int i = 0; i < TAM_VETOR; i++)
        if (!enfileira(&f, dados[i])) return 1;
    if (enfileira(&f, 99)) return 1;
    printf("Saída FIFO: ");
    for (int i = 0; i < TAM_VETOR; i++) {
        int x;
        if (!desenfileira(&f, &x) || x != dados[i]) return 1;
        printf("%s%d", i ? " " : "", x);
    }
    putchar('\n');
    int x;
    if (desenfileira(&f, &x)) return 1;
    return enfileira(&f, 77) && desenfileira(&f, &x) && x == 77 ? 0 : 1;
}
