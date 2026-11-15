#include <stdio.h>
#include <stdlib.h>
typedef struct {
    int v;
    int w;
    int custo;
} Aresta;

void inicializa(int familia[], int tamanho[], int n) {
    for (int i = 0; i < n; i++) {
        familia[i] = i; // no começo, cada vértice é sua própria família
        tamanho[i] = 1;  // cada componente começa com tamanho 1
    }
}

void union_f(int familia[], int tamanho[], int n, int A, int B) { //union é palavra reservada
    if (A == B) return;
    int maior, menor;
    if (tamanho[A] >= tamanho[B]) {
        maior = A;
        menor = B;
    } else {
        maior = B;
        menor = A;
    }
//aqui em cima já conseguimos quem é maior ou menor
    for (int i = 0; i < n; i++) {
        if (familia[i] == menor) {
            familia[i] = maior;
        }
    }
//aqui varremos o vetor todo e mudamos quem é do menor para ser do maior
//na próxima, vamos só mudar os tamanhos, pois somamos os dois agora, já que unimos
    tamanho[maior] += tamanho[menor];
    tamanho[menor] = 0;
}
int comparar_arestas(const void *a, const void *b) {
    Aresta *arestaA = (Aresta *)a;
    Aresta *arestaB = (Aresta *)b;
    return arestaA->custo - arestaB->custo;
} //a ideia aqui é trazer os custos das arestas. Como esse comparador é usado na qsort, ele precisa do const void. Porém, o foco do código não é a ordenação, e sim o Kruskal abaixo

void kruskal(int n, int m, Aresta arestas[]) { // Aresta arestas[] é um vetor de arestas
    int familia[n];
    int tamanho[n];
    Aresta arv_min[n - 1]; // guarda as arestas da árvore geradora mínima

    // 1) Ordenar as arestas em ordem crescente de custo
    qsort(arestas, m, sizeof(Aresta), comparar_arestas); //como no caso o foco não é a ordenação em si, ao invés de montar um algoritmo (que poderia ser o de escolha do usuário), utilizei a função nativa do C qsort, que já resolve o problema

    // 2) Inicializar o vetor do union-find
    inicializa(familia, tamanho, n);

    // 3) Percorrer as arestas em ordem crescente até adicionar n-1 arestas
    int arestas_adicionadas = 0;
    int custo_total = 0;

    for (int i = 0; i < m && arestas_adicionadas < n - 1; i++) {
        int v = arestas[i].v;
        int w = arestas[i].w;

        int fam_v = familia[v];
        int fam_w = familia[w];

        // 3.1) Se familia[v] != familia[w]
        if (fam_v != fam_w) {
            // 3.1.1) Adicione A(v, w) no grafo (árvore resultante)
            arv_min[arestas_adicionadas] = arestas[i];
            arestas_adicionadas++;
            custo_total += arestas[i].custo;

            // 3.1.2) Faça union_f(familia[v], familia[w])
            union_f(familia, tamanho, n, fam_v, fam_w);
        }
    }

if (arestas_adicionadas < n - 1) {
        printf("O grafo original nao e conexo; nao existe arvore geradora.\n");
    } else {
        printf("Arestas da Arvore Geradora de Menor Custo:\n");
        for (int i = 0; i < n - 1; i++) {
            printf("(%d, %d) - custo: %d\n", arv_min[i].v, arv_min[i].w, arv_min[i].custo);
        }
        printf("Custo total minimo: %d\n", custo_total);
    }
}

int main() {
    int n, m;

    if (scanf("%d %d", &n, &m) != 2 || n <= 0 || m < 0) {
        printf("Entrada invalida.\n");
        return 1;
    }

    Aresta *arestas = (Aresta *)malloc(m * sizeof(Aresta));
    if (m > 0 && !arestas) {
        printf("Erro ao alocar memoria para as arestas.\n");
        return 1;
    }

    for (int i = 0; i < m; i++) {
        if (scanf("%d %d %d", &arestas[i].v, &arestas[i].w, &arestas[i].custo) != 3) {
            printf("Erro na leitura da aresta %d.\n", i + 1);
            free(arestas);
            return 1;
        }
    }

    kruskal(n, m, arestas);

    free(arestas);
    return 0;
}
