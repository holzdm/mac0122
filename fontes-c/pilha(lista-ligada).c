 typedef struct No {
    char dado;
    struct No* proximo;
} No; 

void empilha(char y) {
    No* novo = (No*) malloc(sizeof(No));
    novo->dado = y;
    novo->proximo = topo;
    topo = novo;
}  

char desempilha(void) {
    if (topo == NULL) {
        printf("Erro: A pilha esta vazia!\n");
        return '\0';
    }
    
    No* temp = topo;
    char letra = temp->dado;
    topo = temp->proximo;
    free(temp);
    
    return letra;
}
if (topo == NULL) { ... }

No* temp = topo;

char letra = temp->dado;

topo = temp->proximo;
