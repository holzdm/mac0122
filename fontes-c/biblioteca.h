#ifndef MAC0122_BIBLIOTECA_H
#define MAC0122_BIBLIOTECA_H

#include <stddef.h>

enum { TAM_VETOR = 15, TAM_NOMES = 12, TAM_LISTA = 12,
       NUM_VERTICES = 12, NUM_ARESTAS = 18 };

typedef struct No {
    char dado;
    struct No *proximo;
} No;

typedef struct {
    int v;
    int w;
    int custo;
} Aresta;

extern const char *const nomes_exemplo[TAM_NOMES];
extern const Aresta arestas_exemplo[NUM_ARESTAS];

void copia_vetor(int destino[TAM_VETOR]);
void copia_vetor_ordenado(int destino[TAM_VETOR]);
int vetor_ordenado(const int vetor[], size_t n);
void imprime_vetor(const int vetor[], size_t n);
int demonstra_ordenacao(const char *nome, void (*ordena)(int vetor[], int n));

No *cria_lista_exemplo(void);
void libera_lista(No *inicio);
void imprime_lista(const No *inicio);

void copia_grafo(int grafo[NUM_VERTICES][NUM_VERTICES]);

#endif
