#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M 101

/* Extraído de fontes-txt/hashing(strings).txt. */
typedef struct NoHash {
    char *palavra;
    int frequencia;
    struct NoHash *prox;
} NoHash;

typedef struct { NoHash *lista[M]; } Tabela;

unsigned hash(const char *s) {
    unsigned h = 0;
    while (*s) h = (31u * h + (unsigned char)*s++) % M;
    return h;
}

void inicializa(Tabela *t) {
    for (int i = 0; i < M; i++) t->lista[i] = NULL;
}

NoHash *busca(Tabela *t, const char *s) {
    unsigned h = hash(s);
    for (NoHash *p = t->lista[h]; p; p = p->prox)
        if (strcmp(p->palavra, s) == 0) return p;
    return NULL;
}

int insere(Tabela *t, const char *s) {
    NoHash *p = busca(t, s);
    if (p) {
        p->frequencia++;
        return 1;
    }
    NoHash *novo = malloc(sizeof *novo);
    if (!novo) return 0;
    novo->palavra = malloc(strlen(s) + 1);
    if (!novo->palavra) {
        free(novo);
        return 0;
    }
    strcpy(novo->palavra, s);
    novo->frequencia = 1;
    unsigned h = hash(s);
    novo->prox = t->lista[h];
    t->lista[h] = novo;
    return 1;
}

void libera(Tabela *t) {
    for (int i = 0; i < M; i++) {
        NoHash *p = t->lista[i];
        while (p) {
            NoHash *prox = p->prox;
            free(p->palavra);
            free(p);
            p = prox;
        }
        t->lista[i] = NULL;
    }
}

int main(void) {
    Tabela tabela;
    inicializa(&tabela);
    for (int i = 0; i < TAM_NOMES; i++) {
        if (!insere(&tabela, nomes_exemplo[i])) {
            libera(&tabela);
            return 1;
        }
    }
    int total = 0;
    for (int i = 0; i < M; i++)
        for (NoHash *p = tabela.lista[i]; p; p = p->prox) {
            printf("%s: %d\n", p->palavra, p->frequencia);
            total += p->frequencia;
        }
    int ok = total == TAM_NOMES && busca(&tabela, "Ana") &&
             busca(&tabela, "Ana")->frequencia == 3 && !busca(&tabela, "Zoe");
    libera(&tabela);
    return ok ? 0 : 1;
}
