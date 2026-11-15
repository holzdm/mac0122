No* inicio = NULL;
No* fim = NULL;

 void enfileira(char y) {
    No* novo = (No*) malloc(sizeof(No));
    novo->dado = y;
    novo->proximo = NULL;

    if (inicio == NULL) {
        inicio = novo;
    } else {
        fim->proximo = novo;
    }
    fim = novo;
}
novo->dado = y;

novo->proximo = NULL;

if (inicio == NULL) { inicio = novo; }

else { fim->proximo = novo; }

fim = novo;

char desenfileira(void) {
    if (inicio == NULL) {
        printf("Erro: A fila esta vazia!\n");
        return '\0';
    }

    No* temp = inicio;
    char letra = temp->dado;
    inicio = temp->proximo;

    if (inicio == NULL) {
        fim = NULL;
    }

    free(temp);
    return letra;
}

if (inicio == NULL) { ... }

No* temp = inicio;

char letra = temp->dado;

inicio = temp->proximo;

if (inicio == NULL) { fim = NULL; }
