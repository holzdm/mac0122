void insertionsort(int *vetor, int fim){
    for(int i=1;i<fim;i++){
        int key=vetor[i];
        int j=i-1;
        while (j>=0 && vetor[j]>key){
            vetor[j+1]=vetor[j];
            j--;
        }
    vetor[j+1]=key;
    }    
}
