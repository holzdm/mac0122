#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include "./filter-aux.c"
#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
#endif

typedef enum {
    normal,
    comentario_linha,
    comentario_bloco,
    pragma,
    string,
    colchetes
} estado;

const char *tipos[] = {
    "int", "float", "double", "char", "void", 
    "long", "short", "unsigned", "signed", "struct", "const", 
    NULL
};

const char *comandos[] = {
    "if", "else", "for", "while", 
    "switch", "case", "break", "continue", "return", "default", 
    NULL
};

static void descarrega_buffer(char *buffer, int *i, FILE *saida) {
    if (*i == 0) {
        return;
    }

    buffer[*i] = '\0';
    if (palavra_reservada(buffer, tipos)) {
        fprintf(saida, "<span class=\"tipo\">%s</span>", buffer);
    } else if (palavra_reservada(buffer, comandos)) {
        fprintf(saida, "<span class=\"comando\">%s</span>", buffer);
    } else {
        fputs(buffer, saida);
    }
    *i = 0;
}

void gera_html(FILE *entrada, FILE *saida) {
    estado estado_atual = normal;
    int colorindo = 0;
    char buffer[100] = {0};
    int i = 0;
    int c;
    int c_anterior = 0;

    // cabeçalho do html
    fputs("<!DOCTYPE html>\n", saida);
    fputs("<html lang=\"pt-br\">\n", saida);
    fputs("<head>\n", saida);
    fputs("  <meta charset=\"utf-8\">\n", saida);
    fputs("  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n", saida);
    fputs("  <title>Texto - MAC0122</title>\n", saida);
    fputs("  <link rel=\"stylesheet\" href=\"../../estilo.css\">\n", saida);
    fputs("</head>\n", saida);
    fputs("<body>\n"
          "  <nav class=\"navbar\" aria-label=\"Navegação principal\">\n"
          "    <div class=\"navbar-conteudo\">\n"
          "      <a class=\"navbar-marca\" href=\"../../output/index.html\"><span class=\"chave\">{</span> MAC0122 <span class=\"chave\">}</span></a>\n"
          "      <input type=\"checkbox\" id=\"navbar-toggle\" class=\"navbar-toggle\">\n"
          "      <label for=\"navbar-toggle\" class=\"navbar-botao\" aria-label=\"Abrir menu\">&#9776;</label>\n"
          "      <ul class=\"navbar-menu\">\n"
          "        <li><a href=\"../../output/index.html\">Início</a></li>\n"
          "        <li><a href=\"../../output/tecnicas.html\">Técnicas</a></li>\n"
          "        <li><a href=\"../../output/complexidades.html\">Complexidade</a></li>\n"
          "        <li><a href=\"../../output/algoritmos.html\">Algoritmos</a></li>\n"
          "        <li><a href=\"../../output/definicoes.html\">Definições</a></li>\n"
          "        <li><a href=\"../../output/autores.html\">Autores</a></li>\n"
          "        <li><a href=\"../../output/datas.html\">Datas</a></li>\n"
          "        <li><a href=\"../../output/conceitos.html\">Conceitos</a></li>\n"
          "        <li><a href=\"../../output/problemas.html\">Problemas</a></li>\n"
          "        <li><a href=\"../../output/referencias.html\">Referências</a></li>\n"
          "      </ul>\n"
          "    </div>\n"
          "  </nav>\n"
          "  <a class=\"voltar pagina-voltar\" href=\"../../output/index.html\">&larr; Voltar ao índice</a>\n"
          "<pre>\n", saida);

    while ((c = fgetc(entrada)) != EOF) {
        if (c == '@') {
            int prox = fgetc(entrada);
            if (prox == 'c') {
                if (colorindo) {
                    descarrega_buffer(buffer, &i, saida);
                    if (estado_atual != normal) {
                        fputs("</span>", saida);
                    }
                }
                colorindo = !colorindo;
                estado_atual = normal;
                c_anterior = 0;
                continue;
            }
            if (prox != EOF) {
                ungetc(prox, entrada);
            }
        }

        if (!colorindo) {
            escreve_char_html(c, saida);
            continue;
        }

        switch (estado_atual) {
            case normal:
                if (isalnum(c) || c == '_') {
                    buffer[i++] = c;
                } else {
                    descarrega_buffer(buffer, &i, saida);

                    switch (c) {
                        case '"':
                            fputs("<span class=\"string\">\"", saida);
                            estado_atual = string;
                            break;

                        case '=':
                            fputs("<span class=\"igual\">=</span>", saida);
                            break;

                        case '#':
                            fputs("<span class=\"pragma\">#", saida);
                            estado_atual = pragma;
                            break;

                        case '[':
                            fputs("<span class=\"colchete\">[</span><span class=\"conteudo_colchete\">", saida);
                            estado_atual = colchetes;
                            break;

                        case '/': {
                            int prox = fgetc(entrada);
                            if (prox == '/') {
                                fputs("<span class=\"comentario\">//", saida);
                                estado_atual = comentario_linha;
                            } else if (prox == '*') {
                                fputs("<span class=\"comentario\">/*", saida);
                                estado_atual = comentario_bloco;
                            } else {
                                if (prox != EOF) {
                                    ungetc(prox, entrada);
                                }
                                escreve_char_html(c, saida);
                            }
                            break;
                        }

                        default:
                            escreve_char_html(c, saida);
                            break;
                    }
                }
                break;

            case colchetes:
                if (c == ']') {
                    fputs("</span><span class=\"colchete\">]</span>", saida);
                    estado_atual = normal;
                } else {
                    escreve_char_html(c, saida);
                }
                break;

            case string:
                escreve_char_html(c, saida);
                if (c == '"' && c_anterior != '\\') {
                    fputs("</span>", saida);
                    estado_atual = normal;
                }
                break;

            case comentario_linha:
                if (c == '\n') {
                    fputs("</span>", saida);
                    escreve_char_html(c, saida);
                    estado_atual = normal;
                } else {
                    escreve_char_html(c, saida);
                }
                break;

            case pragma:
                if (c == '\n') {
                    fputs("</span>", saida);
                    escreve_char_html(c, saida);
                    estado_atual = normal;
                } else {
                    escreve_char_html(c, saida);
                }
                break;

            case comentario_bloco:
                escreve_char_html(c, saida);
                if (c_anterior == '*' && c == '/') {
                    fputs("</span>", saida);
                    estado_atual = normal;
                }
                break;
        }
        c_anterior = c;
    }

    // Descarrega eventual palavra que ficou no buffer antes do EOF
    descarrega_buffer(buffer, &i, saida);

    // Fecha o span caso o arquivo termine no meio de um estado aberto
    if (colorindo && estado_atual != normal) {
        fputs("</span>", saida);
    }

    fputs("\n</pre>\n", saida);
    fputs("</body>\n", saida);
    fputs("</html>\n", saida);
}

int main(void) {
    const char *pasta_entrada = "../fontes-txt";
    const char *pasta_saida = "filter-output";

    #ifdef _WIN32
        _mkdir(pasta_saida);
    #else
        mkdir(pasta_saida, 0755);
    #endif

    DIR *dir = opendir(pasta_entrada);
    if (dir == NULL) {
        perror("Erro ao abrir a pasta de entrada");
        return 1;
    }

    struct dirent *item;
    char caminho_entrada[512];
    char caminho_saida[512];

    while ((item = readdir(dir)) != NULL) {
        if (strcmp(item->d_name, ".") == 0 || strcmp(item->d_name, "..") == 0) {
            continue;
        }

        const char *ext = strrchr(item->d_name, '.');
        if (ext != NULL && strcmp(ext, ".c") == 0) {
            continue;
        }

        snprintf(caminho_entrada, sizeof(caminho_entrada), "%s/%s", pasta_entrada, item->d_name);
        snprintf(caminho_saida, sizeof(caminho_saida), "%s/%s.html", pasta_saida, item->d_name);

        FILE *entrada = fopen(caminho_entrada, "r");
        if (entrada == NULL) {
            continue;
        }

        FILE *saida = fopen(caminho_saida, "w");
        if (saida == NULL) {
            fclose(entrada);
            continue;
        }

        gera_html(entrada, saida);
        printf("Gerado: %s\n", caminho_saida);

        fclose(entrada);
        fclose(saida);
    }

    closedir(dir);
    return 0;
}
