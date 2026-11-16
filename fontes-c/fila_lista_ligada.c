#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    No *inicio;
    No *fim;
} FilaLista;

/* Extraído de fontes-txt/fila(lista-ligada).txt. */
int enfileira(FilaLista *fila, char y) {
    No *novo = malloc(sizeof *novo);
    if (!novo) return 0;
    novo->dado = y;
    novo->proximo = NULL;
    if (fila->inicio == NULL) fila->inicio = novo;
    else fila->fim->proximo = novo;
    fila->fim = novo;
    return 1;
}

int desenfileira(FilaLista *fila, char *letra) {
    if (!fila->inicio) return 0;
    No *temp = fila->inicio;
    *letra = temp->dado;
    fila->inicio = temp->proximo;
    if (!fila->inicio) fila->fim = NULL;
    free(temp);
    return 1;
}

int main(void) {
    FilaLista fila = { .inicio = cria_lista_exemplo(), .fim = NULL };
    if (!fila.inicio) return 1;
    fila.fim = fila.inicio;
    while (fila.fim->proximo) fila.fim = fila.fim->proximo;
    if (!enfileira(&fila, 'M')) {
        libera_lista(fila.inicio);
        return 1;
    }
    printf("Saída FIFO: ");
    for (char esperado = 'A'; esperado <= 'M'; esperado++) {
        char letra;
        if (!desenfileira(&fila, &letra) || letra != esperado) {
            libera_lista(fila.inicio);
            return 1;
        }
        printf("%c%s", letra, esperado == 'M' ? "\n" : " ");
    }
    char letra;
    return desenfileira(&fila, &letra) || fila.fim != NULL ? 1 : 0;
}
