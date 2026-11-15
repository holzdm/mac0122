void selectionsort(int *v, int fim){
    for(int i=0;i<fim-1;i++){
        int min = i;
        for(int j=i+1; j<fim-1; j++){
            if(v[j]<v[min]) min = j;
        }
        if(min != i){
            int aux = v[i];
            v[i]=v[min];
            v[min]=aux;
        }
    }
}
