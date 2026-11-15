int grafo[6][6] = {
    {  0, 1, 1, 0, 0, 0 }, 
    {  0, 0, 0, 0, 0, 1 }, 
    {  0, 0, 0, 1, 0, 0 }, 
    {  0, 0, 0, 0, 1, 0 }, 
    {  0, 0, 0, 0, 0, 1 }, 
    {  0, 0, 0, 0, 0, 0 }  
};

int pilha[6];
int t = 0; 

int visitado[6]; 
int pai[6];

void busca_em_profundidade(int origem) {
    for (int i = 0; i < 6; i++) {
        visitado[i] = 0;
        pai[i] = -1;
    }

    visitado[origem] = 1;
    pai[origem] = origem;
    pilha[t++] = origem;

    printf("Ordem em que os vertices sairam da pilha (DFS): ");

    while (t != 0) {
        int x = pilha[--t];
        printf("%d ", x);

        for (int y = 0; y < V; y++) {
            
            if (grafo[x][y] == 1 && visitado[y] == 0) {
                visitado[y] = 1; 
                pai[y] = x;     
                pilha[t++] = y;  
            }
        }
    }
    printf("\n\n");
}

void imprime_caminho_direto(int origem, int destino) {
    if (visitado[destino] == 0) {
        printf("Nao existe caminho de %d ate %d.\n", origem, destino);
        return;
    }

    printf("Caminho encontrado pela Pilha (voltando de %d ate %d): ", destino, origem);

    int atual = destino;
    while (atual != origem) {
        printf("%d <- ", atual);
        atual = pai[atual];
    }
    printf("%d\n", origem);
}
