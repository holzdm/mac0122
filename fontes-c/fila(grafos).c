int grafo[6][6] = {
    {  0, 1, 1, 0, 0, 0 },
    {  0, 0, 0, 1, 1, 0 },
    {  0, 0, 0, 0, 1, 1 },
    {  0, 0, 0, 0, 0, 1 },
    {  0, 0, 0, 0, 0, 1 },
    {  0, 0, 0, 0, 0, 0 }
};

int fila[6];
int p = 0, u = 0;
int dist[6];
int pai[6];

void busca_em_largura(int origem) {
    for (int i = 0; i < 6; i++) {
        dist[i] = -1;
        pai[i] = -1;
    }

    dist[origem] = 0;
    pai[origem] = origem;
    fila[u++] = origem;

    while (p != u) {
        int x = fila[p++];

        for (int y = 0; y < 6; y++) {
            if (grafo[x][y] == 1 && dist[y] == -1) {
                dist[y] = dist[x] + 1;
                pai[y] = x;
                fila[u++] = y;
            }
        }
    }
}

if (grafo[x][y] == 1 && dist[y] == -1) :

void imprime_caminho(int origem, int destino) {
    if (dist[destino] == -1) {
        printf("Nao existe caminho de %d ate %d.\n", origem, destino);
        return;
    }

    int caminho[V];
    int t = 0;

    int atual = destino;
    while (atual != origem) {
        caminho[t++] = atual;
        atual = pai[atual];
    }
    caminho[t++] = origem;

    printf("Menor caminho de %d ate %d (%d passos): ", origem, destino, dist[destino]);
    while (t != 0) {
        printf("%d", caminho[--t]);
        if (t != 0) printf(" -> ");
    }
    printf("\n");
}
