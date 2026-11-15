#include <stdio.h>
void bubblesort(int *vetor, int fim){
    int aux=0;
    while(fim > 1){
        for (int i = 0; i < fim-1 ; i++){
        if(vetor[i]>vetor[i+1]){
            aux=vetor[i];
            vetor[i]=vetor[i+1];
            vetor[i+1]=aux;
        }
    }
    fim--;
    }
    
}
