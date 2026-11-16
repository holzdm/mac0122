# mac0122
Trabalho da disciplina MAC0122 - Princípios de desenvolvimento de algorítmos.

## To-Do:

### Ordenação de números
- [x] Bubble Sort (vetor)
- [x] Selection Sort (vetor)
- [x] Insertion Sort (vetor) 
- [x] Merge Sort (vetor)
- [x] Heap Sort (vetor) 
- [x] Quick Sort (vetor)
- [x] Radix Sort (vetor)
- [ ] ordenação por tentativa e erro

### Ordenação de nomes
- [ ] Bubble Sort
- [ ] Frequência

### Inserção e Remoção
- [x] Pilha (vetor)
- [x] Pilha (lista ligada)
- [x] Fila (vetor)
- [x] Fila (lista ligada)
- [x] Pilha (grafos)
- [x] Fila (grafos)

### Hashing
- [ ] Espalhamento para conjunto de strings

### Busca
- [ ] Busca binária (vetor)
- [ ] Busca em largura (gráfos)
- [ ] Busca em profundidade (gráfos)

### Definições
- [x] Grafo
- [x] Estruturas de gráfos (árovre, ciclos, conexos, etc)
- [x] Lista ligada
- [ ] Hashing
- [x] Structs

### Misc 
- [ ] hanoi 
- [ ] kruskall
- [ ] djikstra

## Gerar o site

Execute os comandos a partir da raiz do repositório:

```sh
gcc -std=c11 -Wall -Wextra -Wpedantic processing/filter.c -o /tmp/mac0122-filter
(cd processing && /tmp/mac0122-filter)
gcc -std=c11 -Wall -Wextra -Wpedantic processing/index.c -o /tmp/mac0122-index
/tmp/mac0122-index
```

O filtro cria as páginas em `processing/filter-output/`. O indexador lê os HTML dessa pasta, extrai os campos de técnica, algoritmo, autores, data, conceitos e problema, e guarda **todos os índices gerados em `output/`**, incluindo `output/index.html`. Ele usa a capa preservada em `index1.html`; todas as páginas compartilham o tema de `estilo.css`. Execute o filtro antes do indexador quando alterar os arquivos de `fontes-txt/`.

Os textos com o campo `2. Algoritmo:` aparecem em `output/algoritmos.html`. Os textos com `2. Definição:` aparecem em `output/definicoes.html`, que também recebe textos sem título explícito. A página inicial mostra as duas listas separadamente.

O campo `7. Complexidade:` no cabeçalho dos algoritmos registra o tempo no pior caso da implementação apresentada. O índice `output/complexidades.html` agrupa os algoritmos por esse valor. Em filas e pilhas, o custo é por operação; na tabela hash, por busca ou inserção. Use `n` para o tamanho do vetor, `L` para o comprimento máximo de uma string, `V` para vértices, `E` para arestas, `R` para o intervalo de valores do Counting Sort e `N` para a quantidade de palavras distintas na tabela hash.

O indexador também lê `fontes-txt/referencias.txt` e cria `output/referencias.html`, acessível pela navbar. Separe as referências por uma linha vazia; a primeira linha de cada bloco vira o título, e linhas começando com `Disponível em:` viram links quando contêm uma URL HTTP ou HTTPS.
