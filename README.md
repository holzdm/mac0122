# mac0122
Trabalho da disciplina MAC0122 - Princípios de desenvolvimento de algorítmos.

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

## Exemplos em C

Os 21 algoritmos das fichas `fontes-txt/` têm programas independentes em `fontes-c/`. Cada arquivo conserva a função central do respectivo trecho `@c` e inclui um `main` que a executa. Trechos ilustrativos incompletos e erros de índice, alocação ou compilação foram ajustados nesses programas.

`fontes-c/biblioteca.h` e `fontes-c/biblioteca.c` fornecem um vetor desordenado de 15 inteiros, 12 nomes, uma lista ligada de 12 nós e um grafo conectado de 12 vértices com 18 arestas. A busca binária usa uma cópia ordenada do mesmo vetor. A biblioteca é compilada como `fontes-c/libdados.a`.

Para compilar e executar todos os exemplos a partir da raiz do projeto:

```sh
make -C fontes-c all
make -C fontes-c test
./fontes-c/bin/kruskal
```
