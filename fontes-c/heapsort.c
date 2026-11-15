void troca(int a, int b){
    int aux = a;
    a = b
    b = aux;  
}

void constroiHeap (int v[], int fim){
   for (int k = 1; k <= fim; ++k){                   
      int f = k+1;
      while (f > 1 && v[f/2] < v[f]) {  
         troca(v[f/2], v[f]);         
         f /= 2;                        
      }
   }
}

void peneira(int v[],int fim){
    int f=2;
    while(f<=fim){
        if (f<fim && v[f]<v[f+1]) ++f; //escolhe o filho mais valioso de f/2;
        if (v[f/2] >= v[f]) break;
        troca(v[f/2], v[f]);          
        f *= 2; 
    }
}
    

void heapsort(int v[], int fim){
    constroiHeap(v, fim);
    for(int n=fim; n>=2; n--){
        troca(v[1], v[n]);
        peneira(v, n-1);
    }
}
