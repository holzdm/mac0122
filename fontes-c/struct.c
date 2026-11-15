struct pessoa {
int NUSP;
int CPF;
char Letra;
};

struct pessoa {
int NUSP;
int CPF;
char Letra;
};
struct pessoa Mateus; //cria uma struct da forma mostrada acima, com nome Mateus
struct pessoa x; // cria uma struct da forma mostrada acima, com nome x

Mateus.NUSP = 12345678;

typedef struct pessoa ficha; //cria um tipo de dados com a struct que foi construída
ficha Mateus; //cria uma ficha (tipo de dados) chamada "Mateus"

ficha *p; //definimos um ponteiro p que aponta para uma struct
ficha Mateus;
p = &Mateus; //agora, p aponta para a ficha Mateus
//posso usar o ponto para acessar variáveis da mesma forma:
(*p).NUSP = 12345678; //define o NUSP de Mateus como 12345678
