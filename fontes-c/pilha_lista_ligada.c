#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct { No *topo; } PilhaLista;

/* Extraído de fontes-txt/pilha(lista-ligada).txt. */
int empilha(PilhaLista *pilha, char y) {
    No *novo = malloc(sizeof *novo);
    if (!novo) return 0;
    novo->dado = y;
    novo->proximo = pilha->topo;
    pilha->topo = novo;
    return 1;
}

int desempilha(PilhaLista *pilha, char *letra) {
    if (!pilha->topo) return 0;
    No *temp = pilha->topo;
    *letra = temp->dado;
    pilha->topo = temp->proximo;
    free(temp);
    return 1;
}

int main(void) {
    PilhaLista pilha = { .topo = cria_lista_exemplo() };
    if (!pilha.topo) return 1;
    if (!empilha(&pilha, 'M')) {
        libera_lista(pilha.topo);
        return 1;
    }
    char letra;
    if (!desempilha(&pilha, &letra) || letra != 'M') {
        libera_lista(pilha.topo);
        return 1;
    }
    printf("Saída LIFO: M ");
    for (char esperado = 'A'; esperado <= 'L'; esperado++) {
        if (!desempilha(&pilha, &letra) || letra != esperado) {
            libera_lista(pilha.topo);
            return 1;
        }
        printf("%c%s", letra, esperado == 'L' ? "\n" : " ");
    }
    return desempilha(&pilha, &letra) ? 1 : 0;
}
