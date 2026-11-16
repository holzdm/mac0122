#include "biblioteca.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const int vetor_exemplo[TAM_VETOR] = {
    37, 4, 18, 0, 25, 9, 4, 42, 13, 31, 7, 18, 1, 29, 16
};

const char *const nomes_exemplo[TAM_NOMES] = {
    "Carlos", "Ana", "Bruno", "Ana", "Dora", "Bia",
    "Carlos", "Eva", "Bia", "Ana", "Dora", "Fabi"
};

static const char dados_lista[TAM_LISTA] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L'
};

const Aresta arestas_exemplo[NUM_ARESTAS] = {
    {0, 1, 4}, {0, 2, 3}, {1, 2, 1}, {1, 3, 2},
    {2, 4, 7}, {3, 4, 2}, {3, 5, 3}, {4, 5, 1},
    {4, 6, 6}, {5, 7, 4}, {6, 7, 2}, {6, 8, 5},
    {7, 9, 3}, {8, 9, 2}, {8, 10, 4}, {9, 10, 1},
    {9, 11, 5}, {10, 11, 2}
};

static int compara_int(const void *a, const void *b) {
    int x = *(const int *)a;
    int y = *(const int *)b;
    return (x > y) - (x < y);
}

void copia_vetor(int destino[TAM_VETOR]) {
    for (size_t i = 0; i < TAM_VETOR; i++) destino[i] = vetor_exemplo[i];
}

void copia_vetor_ordenado(int destino[TAM_VETOR]) {
    copia_vetor(destino);
    qsort(destino, TAM_VETOR, sizeof destino[0], compara_int);
}

int vetor_ordenado(const int vetor[], size_t n) {
    for (size_t i = 1; i < n; i++)
        if (vetor[i - 1] > vetor[i]) return 0;
    return 1;
}

void imprime_vetor(const int vetor[], size_t n) {
    for (size_t i = 0; i < n; i++)
        printf("%s%d", i ? " " : "", vetor[i]);
    putchar('\n');
}

int demonstra_ordenacao(const char *nome, void (*ordena)(int vetor[], int n)) {
    int vetor[TAM_VETOR];
    int esperado[TAM_VETOR];
    copia_vetor(vetor);
    copia_vetor_ordenado(esperado);
    printf("%s\nEntrada: ", nome);
    imprime_vetor(vetor, TAM_VETOR);
    ordena(vetor, TAM_VETOR);
    printf("Saída:   ");
    imprime_vetor(vetor, TAM_VETOR);
    if (memcmp(vetor, esperado, sizeof vetor) != 0) {
        fputs("Ordenação incorreta.\n", stderr);
        return EXIT_FAILURE;
    }
    for (int n = 0; n <= TAM_VETOR; n++) {
        for (int modo = 0; modo < 4; modo++) {
            int entrada[TAM_VETOR];
            int referencia[TAM_VETOR];
            for (int i = 0; i < n; i++) {
                if (modo == 0) entrada[i] = i;
                else if (modo == 1) entrada[i] = n - i;
                else if (modo == 2) entrada[i] = 7;
                else entrada[i] = (i * 17 + n * 11) % 43;
            }
            memcpy(referencia, entrada, (size_t)n * sizeof entrada[0]);
            qsort(referencia, (size_t)n, sizeof referencia[0], compara_int);
            ordena(entrada, n);
            if (memcmp(entrada, referencia, (size_t)n * sizeof entrada[0]) != 0) {
                fprintf(stderr, "%s falhou com %d elementos no caso %d.\n", nome, n, modo);
                return EXIT_FAILURE;
            }
        }
    }
    return EXIT_SUCCESS;
}

No *cria_lista_exemplo(void) {
    No *inicio = NULL;
    No **fim = &inicio;
    for (size_t i = 0; i < TAM_LISTA; i++) {
        No *novo = malloc(sizeof *novo);
        if (!novo) {
            libera_lista(inicio);
            return NULL;
        }
        novo->dado = dados_lista[i];
        novo->proximo = NULL;
        *fim = novo;
        fim = &novo->proximo;
    }
    return inicio;
}

void libera_lista(No *inicio) {
    while (inicio) {
        No *proximo = inicio->proximo;
        free(inicio);
        inicio = proximo;
    }
}

void imprime_lista(const No *inicio) {
    for (const No *p = inicio; p; p = p->proximo)
        printf("%c%s", p->dado, p->proximo ? " -> " : "\n");
}

void copia_grafo(int grafo[NUM_VERTICES][NUM_VERTICES]) {
    for (int i = 0; i < NUM_VERTICES; i++)
        for (int j = 0; j < NUM_VERTICES; j++) grafo[i][j] = 0;
    for (int i = 0; i < NUM_ARESTAS; i++) {
        int v = arestas_exemplo[i].v;
        int w = arestas_exemplo[i].w;
        grafo[v][w] = 1;
        grafo[w][v] = 1;
    }
}
