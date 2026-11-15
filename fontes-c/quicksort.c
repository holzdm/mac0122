void quicksort(int *vetor, int inicio, int fim){
    if (inicio>=fim) return;
    int pivo=vetor[fim];
    int maioresq=inicio;
    int menordir=fim-1;
    int aux=0;

    while (maioresq<=menordir){
        while(maioresq<=menordir && vetor[maioresq] <= pivo){
            maioresq++;
        }
        while(maioresq<=menordir && vetor[menordir] >= pivo){
            menordir--;
        }

        if (maioresq < menordir){
            aux = vetor[maioresq];
            vetor[maioresq] = vetor[menordir];
            vetor[menordir] = aux;
        }
    }
    vetor[fim]=vetor[maioresq];
    vetor[maioresq]=pivo;
    quicksort(vetor, inicio, maioresq-1);
    quicksort(vetor, maioresq+1, fim);
}
