#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#define MAX_LINHA 2048
#define MAX_TEXTO 512
#define MAX_PROBLEMA 1024
#define PASTA_SAIDA "output"

typedef struct {
    char arquivo_html[512];
    bool definicao;
    char tecnica[MAX_TEXTO];
    char nome[MAX_TEXTO];
    char autores[MAX_TEXTO];
    char data[64];
    char conceitos[MAX_TEXTO];
    char problema[MAX_PROBLEMA];
    char complexidade[MAX_TEXTO];
} AlgoritmoInfo;

typedef struct {
    char titulo[MAX_TEXTO];
    char slug[MAX_TEXTO];
    int *indices_algoritmos;
    int qtd_algoritmos;
    int cap_algoritmos;
} GrupoIndice;

typedef struct {
    char nome_plural[64];     /* Ex: "Técnicas" */
    char nome_singular[64];   /* Ex: "Técnica" */
    char arquivo_indice[64];  /* Ex: "tecnicas.html" */
    char prefixo_pagina[64];  /* Ex: "tecnica" -> tecnica_metodo_guloso.html */
    GrupoIndice *grupos;
    int qtd_grupos;
    int cap_grupos;
} Taxonomia;

/* Ponteiro global auxiliar para ordenação de índices com qsort padrão */
static const AlgoritmoInfo *g_algoritmos_ref = NULL;

/* Ignora espaços, quebras de linha, BOM UTF-8 e zero-width space (\u200b) */
static const char *pular_invisiveis(const char *s) {
    const unsigned char *p = (const unsigned char *)s;
    while (*p != '\0') {
        if (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') {
            p++;
        } else if (p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF) {
            p += 3;
        } else if (p[0] == 0xE2 && p[1] == 0x80 && p[2] == 0x8B) {
            p += 3;
        } else {
            break;
        }
    }
    return (const char *)p;
}

static void limpar_fim(char *s) {
    size_t len = strlen(s);
    while (len > 0) {
        unsigned char c = (unsigned char)s[len - 1];
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            s[--len] = '\0';
        } else if (len >= 3 &&
                   (unsigned char)s[len - 3] == 0xE2 &&
                   (unsigned char)s[len - 2] == 0x80 &&
                   (unsigned char)s[len - 1] == 0x8B) {
            len -= 3;
            s[len] = '\0';
        } else {
            break;
        }
    }
}

/* Converte textos UTF-8 com acentos/espaços em nomes seguros de arquivo (slugs) */
static void gerar_slug(const char *entrada, char *saida, size_t max_len) {
    const unsigned char *p = (const unsigned char *)entrada;
    size_t j = 0;
    bool ultimo_underscore = true;

    while (*p != '\0' && j + 1 < max_len) {
        char c_ascii = '\0';

        if (*p == 0xC2 && p[1] == 0xB2) {
            c_ascii = '2';
            p += 2;
        } else if (*p == 0xC3 && p[1] != '\0') {
            unsigned char seg = p[1];
            if ((seg >= 0x80 && seg <= 0x85) || (seg >= 0xA0 && seg <= 0xA5)) c_ascii = 'a';
            else if (seg == 0x87 || seg == 0xA7) c_ascii = 'c';
            else if ((seg >= 0x88 && seg <= 0x8B) || (seg >= 0xA8 && seg <= 0xAB)) c_ascii = 'e';
            else if ((seg >= 0x8C && seg <= 0x8F) || (seg >= 0xAC && seg <= 0xAF)) c_ascii = 'i';
            else if (seg == 0x91 || seg == 0xB1) c_ascii = 'n';
            else if ((seg >= 0x92 && seg <= 0x96) || (seg >= 0xB2 && seg <= 0xB6)) c_ascii = 'o';
            else if ((seg >= 0x99 && seg <= 0x9C) || (seg >= 0xB9 && seg <= 0xBC)) c_ascii = 'u';
            p += 2;
        } else if (*p < 0x80) {
            if (isalnum(*p)) {
                c_ascii = (char)tolower(*p);
            }
            p += 1;
        } else {
            p += 1;
        }

        if (c_ascii != '\0') {
            saida[j++] = c_ascii;
            ultimo_underscore = false;
        } else if (!ultimo_underscore) {
            saida[j++] = '_';
            ultimo_underscore = true;
        }
    }

    if (j > 0 && saida[j - 1] == '_') {
        j--;
    }
    saida[j] = '\0';

    if (j == 0) {
        strncpy(saida, "geral", max_len - 1);
        saida[max_len - 1] = '\0';
    }
}

/* Os nomes gerados pelo filtro podem terminar em .txt.html ou apenas .html. */
static void titulo_do_arquivo(const char *nome_arquivo, char *destino, size_t max_len) {
    snprintf(destino, max_len, "%s", nome_arquivo);
    size_t len = strlen(destino);
    if (len >= 5 && strcmp(destino + len - 5, ".html") == 0) {
        destino[len - 5] = '\0';
        len -= 5;
    }
    if (len >= 4 && strcmp(destino + len - 4, ".txt") == 0) {
        destino[len - 4] = '\0';
    }
    for (char *p = destino; *p; p++) {
        if (*p == '_') *p = ' ';
    }
    if (destino[0] != '\0') {
        destino[0] = (char)toupper((unsigned char)destino[0]);
    }
}

/* Extrai texto de uma linha do <pre>, removendo spans e entidades HTML comuns. */
static void texto_do_html(const char *origem, char *destino, size_t max_len) {
    size_t j = 0;
    bool dentro_tag = false;
    for (size_t i = 0; origem[i] && j + 1 < max_len; i++) {
        if (origem[i] == '<') { dentro_tag = true; continue; }
        if (dentro_tag) {
            if (origem[i] == '>') dentro_tag = false;
            continue;
        }
        if (origem[i] == '&') {
            struct { const char *entidade; char caractere; } entidades[] = {
                {"&amp;", '&'}, {"&lt;", '<'}, {"&gt;", '>'},
                {"&quot;", '"'}, {"&#39;", '\''}, {"&#60;", '<'}, {"&#62;", '>'}
            };
            bool encontrou = false;
            for (size_t k = 0; k < sizeof(entidades) / sizeof(entidades[0]); k++) {
                size_t n = strlen(entidades[k].entidade);
                if (strncmp(origem + i, entidades[k].entidade, n) == 0) {
                    destino[j++] = entidades[k].caractere;
                    i += n - 1;
                    encontrou = true;
                    break;
                }
            }
            if (encontrou) continue;
        }
        destino[j++] = origem[i];
    }
    destino[j] = '\0';
}

static bool extrair_campo(const char *linha, const char *chave, char *destino, size_t max_len) {
    const char *p = pular_invisiveis(linha);
    size_t len_chave = strlen(chave);

    if (strncmp(p, chave, len_chave) == 0) {
        p = pular_invisiveis(p + len_chave);
        strncpy(destino, p, max_len - 1);
        destino[max_len - 1] = '\0';
        limpar_fim(destino);
        return true;
    }
    return false;
}

static bool ler_arquivo_html(const char *caminho, const char *nome_arquivo,
                             AlgoritmoInfo *alg) {
    FILE *fp = fopen(caminho, "r");
    if (!fp) {
        fprintf(stderr, "Aviso: nao foi possivel abrir '%s'\n", caminho);
        return false;
    }

    memset(alg, 0, sizeof(AlgoritmoInfo));
    snprintf(alg->arquivo_html, sizeof(alg->arquivo_html), "%s", caminho);

    char linha[MAX_LINHA];
    bool dentro_pre = false;
    int linhas_metadados = 0;
    while (fgets(linha, sizeof(linha), fp) != NULL) {
        if (!dentro_pre) {
            if (strstr(linha, "<pre") != NULL) dentro_pre = true;
            continue;
        }
        if (strstr(linha, "</pre>") != NULL || linhas_metadados++ >= 12) break;

        char texto[MAX_LINHA];
        texto_do_html(linha, texto, sizeof(texto));
        if (extrair_campo(texto, "1. Técnica:", alg->tecnica, sizeof(alg->tecnica))) continue;
        if (extrair_campo(texto, "1. Tecnica:", alg->tecnica, sizeof(alg->tecnica))) continue;
        if (extrair_campo(texto, "2. Algoritmo:", alg->nome, sizeof(alg->nome))) continue;
        if (extrair_campo(texto, "2. Definição:", alg->nome, sizeof(alg->nome))) {
            alg->definicao = true;
            continue;
        }
        if (extrair_campo(texto, "3. Autores:", alg->autores, sizeof(alg->autores))) continue;
        if (extrair_campo(texto, "3. Autor:", alg->autores, sizeof(alg->autores))) continue;
        if (extrair_campo(texto, "4. Data:", alg->data, sizeof(alg->data))) continue;
        if (extrair_campo(texto, "5. Conceitos:", alg->conceitos, sizeof(alg->conceitos))) continue;
        if (extrair_campo(texto, "Conceitos:", alg->conceitos, sizeof(alg->conceitos))) continue;
        if (extrair_campo(texto, "6. Problema:", alg->problema, sizeof(alg->problema))) continue;
        if (extrair_campo(texto, "Problema:", alg->problema, sizeof(alg->problema))) continue;
        if (extrair_campo(texto, "7. Complexidade:", alg->complexidade, sizeof(alg->complexidade))) continue;
    }

    fclose(fp);

    /* Mantém o nome do arquivo como título para definições sem cabeçalho próprio. */
    if (alg->nome[0] == '\0') {
        alg->definicao = true;
        titulo_do_arquivo(nome_arquivo, alg->nome, sizeof(alg->nome));
    }
    return true;
}

static void inicializar_taxonomia(Taxonomia *tax, const char *plural, const char *singular,
                                  const char *arquivo, const char *prefixo) {
    memset(tax, 0, sizeof(Taxonomia));
    strncpy(tax->nome_plural, plural, sizeof(tax->nome_plural) - 1);
    strncpy(tax->nome_singular, singular, sizeof(tax->nome_singular) - 1);
    strncpy(tax->arquivo_indice, arquivo, sizeof(tax->arquivo_indice) - 1);
    strncpy(tax->prefixo_pagina, prefixo, sizeof(tax->prefixo_pagina) - 1);
}

static void adicionar_ao_grupo(GrupoIndice *grupo, int idx_alg) {
    for (int i = 0; i < grupo->qtd_algoritmos; i++) {
        if (grupo->indices_algoritmos[i] == idx_alg) {
            return; /* Evita duplicatas no mesmo grupo */
        }
    }

    if (grupo->qtd_algoritmos == grupo->cap_algoritmos) {
        int nova_cap = (grupo->cap_algoritmos == 0) ? 8 : grupo->cap_algoritmos * 2;
        int *novo = realloc(grupo->indices_algoritmos, (size_t)nova_cap * sizeof(int));
        if (!novo) {
            perror("Erro de memoria");
            exit(EXIT_FAILURE);
        }
        grupo->indices_algoritmos = novo;
        grupo->cap_algoritmos = nova_cap;
    }

    grupo->indices_algoritmos[grupo->qtd_algoritmos++] = idx_alg;
}

static void registrar_chave_taxonomia(Taxonomia *tax, const char *valor_bruto, int idx_alg) {
    char slug[MAX_TEXTO];
    gerar_slug(valor_bruto, slug, sizeof(slug));

    for (int i = 0; i < tax->qtd_grupos; i++) {
        if (strcmp(tax->grupos[i].slug, slug) == 0) {
            adicionar_ao_grupo(&tax->grupos[i], idx_alg);
            return;
        }
    }

    if (tax->qtd_grupos == tax->cap_grupos) {
        int nova_cap = (tax->cap_grupos == 0) ? 8 : tax->cap_grupos * 2;
        GrupoIndice *novo = realloc(tax->grupos, (size_t)nova_cap * sizeof(GrupoIndice));
        if (!novo) {
            perror("Erro de memoria");
            exit(EXIT_FAILURE);
        }
        tax->grupos = novo;
        tax->cap_grupos = nova_cap;
    }

    GrupoIndice *g = &tax->grupos[tax->qtd_grupos++];
    memset(g, 0, sizeof(GrupoIndice));
    strncpy(g->titulo, valor_bruto, sizeof(g->titulo) - 1);
    strncpy(g->slug, slug, sizeof(g->slug) - 1);
    adicionar_ao_grupo(g, idx_alg);
}

/* Divide campos com múltiplos valores separados por vírgula ou ponto-e-vírgula */
static void indexar_campo_multiplo(Taxonomia *tax, const char *campo_texto, int idx_alg) {
    if (campo_texto[0] == '\0') return;

    char copia[MAX_TEXTO];
    strncpy(copia, campo_texto, sizeof(copia) - 1);
    copia[sizeof(copia) - 1] = '\0';

    char *token = strtok(copia, ",;");
    while (token != NULL) {
        char item[MAX_TEXTO];
        const char *limpo = pular_invisiveis(token);
        strncpy(item, limpo, sizeof(item) - 1);
        item[sizeof(item) - 1] = '\0';
        limpar_fim(item);

        if (item[0] != '\0') {
            registrar_chave_taxonomia(tax, item, idx_alg);
        }
        token = strtok(NULL, ",;");
    }
}

/* Autores podem ser separados por vírgula, barra ou " e ". Observações entre
   parênteses não fazem parte do nome indexado. */
static void indexar_autores(Taxonomia *tax, const char *texto, int idx_alg) {
    if (!texto[0]) return;
    if (strstr(texto, "sem atribuição histórica") != NULL) {
        char descricao[MAX_TEXTO];
        size_t tamanho = strcspn(texto, "(");
        if (tamanho >= sizeof(descricao)) tamanho = sizeof(descricao) - 1;
        memcpy(descricao, texto, tamanho);
        descricao[tamanho] = '\0';
        limpar_fim(descricao);
        if (descricao[0]) registrar_chave_taxonomia(tax, descricao, idx_alg);
        return;
    }

    char autor[MAX_TEXTO];
    size_t j = 0;
    int parenteses = 0;
    bool ignorar_sufixo = false;
    for (size_t i = 0;; i++) {
        char c = texto[i];
        if (parenteses > 0) {
            if (c == '\0') break;
            if (c == '(') parenteses++;
            else if (c == ')') parenteses--;
            continue;
        }

        bool separador = c == '\0' ||
            c == '(' || c == ',' || c == ';' || c == '/' ||
            (c == ' ' && texto[i + 1] == 'e' && texto[i + 2] == ' ');
        if (separador) {
            if (!ignorar_sufixo) {
                autor[j] = '\0';
                const char *limpo = pular_invisiveis(autor);
                char nome[MAX_TEXTO];
                snprintf(nome, sizeof(nome), "%s", limpo);
                limpar_fim(nome);
                if (nome[0]) registrar_chave_taxonomia(tax, nome, idx_alg);
            }
            j = 0;
            if (c == '\0') break;
            if (c == '(') {
                parenteses = 1;
                ignorar_sufixo = true;
            } else {
                ignorar_sufixo = false;
                if (c == ' ') i += 2; /* salta o " e " */
            }
            continue;
        }
        if (!ignorar_sufixo && j + 1 < sizeof(autor)) autor[j++] = c;
    }
}

static int comparar_grupos(const void *a, const void *b) {
    const GrupoIndice *ga = (const GrupoIndice *)a;
    const GrupoIndice *gb = (const GrupoIndice *)b;
    return strcmp(ga->titulo, gb->titulo);
}

static int comparar_grupos_slug(const void *a, const void *b) {
    const GrupoIndice *ga = (const GrupoIndice *)a;
    const GrupoIndice *gb = (const GrupoIndice *)b;
    return strcmp(ga->slug, gb->slug);
}

static int comparar_indices_algoritmos(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    int cmp = strcmp(g_algoritmos_ref[ia].nome, g_algoritmos_ref[ib].nome);
    return cmp ? cmp : strcmp(g_algoritmos_ref[ia].arquivo_html,
                              g_algoritmos_ref[ib].arquivo_html);
}

static int comparar_algoritmos_direto(const void *a, const void *b) {
    const AlgoritmoInfo *aa = (const AlgoritmoInfo *)a;
    const AlgoritmoInfo *ab = (const AlgoritmoInfo *)b;
    int cmp = strcmp(aa->nome, ab->nome);
    return cmp ? cmp : strcmp(aa->arquivo_html, ab->arquivo_html);
}

/* Os enunciados usam "Palavra-chave: descrição". */
static void separar_problema(const char *problema, char *palavra_chave,
                             size_t tamanho, const char **descricao) {
    const char *separador = strchr(problema, ':');
    if (!separador) {
        snprintf(palavra_chave, tamanho, "Outros");
        *descricao = problema;
        return;
    }

    size_t comprimento = (size_t)(separador - problema);
    if (comprimento >= tamanho) comprimento = tamanho - 1;
    memcpy(palavra_chave, problema, comprimento);
    palavra_chave[comprimento] = '\0';
    limpar_fim(palavra_chave);
    *descricao = pular_invisiveis(separador + 1);
}

static int comparar_indices_problema(const void *a, const void *b) {
    int ia = *(const int *)a;
    int ib = *(const int *)b;
    char palavra_a[MAX_TEXTO], palavra_b[MAX_TEXTO];
    const char *descricao_a, *descricao_b;
    separar_problema(g_algoritmos_ref[ia].problema, palavra_a, sizeof(palavra_a), &descricao_a);
    separar_problema(g_algoritmos_ref[ib].problema, palavra_b, sizeof(palavra_b), &descricao_b);

    char slug_a[MAX_TEXTO], slug_b[MAX_TEXTO];
    gerar_slug(palavra_a, slug_a, sizeof(slug_a));
    gerar_slug(palavra_b, slug_b, sizeof(slug_b));
    int cmp = strcmp(slug_a, slug_b);
    if (!cmp) cmp = strcmp(descricao_a, descricao_b);
    return cmp ? cmp : comparar_indices_algoritmos(a, b);
}

static void ordenar_taxonomia(Taxonomia *tax, const AlgoritmoInfo *algs) {
    g_algoritmos_ref = algs;
    qsort(tax->grupos, (size_t)tax->qtd_grupos, sizeof(GrupoIndice), comparar_grupos);
    for (int i = 0; i < tax->qtd_grupos; i++) {
        qsort(tax->grupos[i].indices_algoritmos,
              (size_t)tax->grupos[i].qtd_algoritmos,
              sizeof(int),
              comparar_indices_algoritmos);
    }
}

/* Escapa caracteres especiais de HTML para evitar quebra de marcação */
static void fputs_html(FILE *fp, const char *texto) {
    for (const char *p = texto; *p != '\0'; p++) {
        switch (*p) {
            case '&':  fputs("&amp;", fp);  break;
            case '<':  fputs("&lt;", fp);   break;
            case '>':  fputs("&gt;", fp);   break;
            case '"':  fputs("&quot;", fp); break;
            case '\'': fputs("&#39;", fp); break;
            default:   fputc(*p, fp);       break;
        }
    }
}

/* Codifica o caminho para que nomes com espaços, acentos e parênteses funcionem no href. */
static void fputs_href(FILE *fp, const char *caminho) {
    const unsigned char *p = (const unsigned char *)caminho;
    for (; *p; p++) {
        if (isalnum(*p) && *p < 128) fputc(*p, fp);
        else if (*p == '/' || *p == '-' || *p == '_' || *p == '.') fputc(*p, fp);
        else fprintf(fp, "%%%02X", *p);
    }
}

static FILE *abrir_saida(const char *nome_arquivo) {
    char caminho[1024];
    int n = snprintf(caminho, sizeof(caminho), "%s/%s", PASTA_SAIDA, nome_arquivo);
    if (n < 0 || (size_t)n >= sizeof(caminho)) {
        fprintf(stderr, "Caminho de saída longo demais: %s\n", nome_arquivo);
        return NULL;
    }
    FILE *fp = fopen(caminho, "w");
    if (!fp) perror(caminho);
    return fp;
}

static void escrever_cabecalho_html(FILE *fp, const char *titulo, const char *pagina_atual) {
    static const struct { const char *arquivo; const char *rotulo; } menu[] = {
        {"index.html", "Início"}, {"tecnicas.html", "Técnicas"},
        {"complexidades.html", "Complexidade"},
        {"algoritmos.html", "Algoritmos"}, {"definicoes.html", "Definições"},
        {"autores.html", "Autores"},
        {"datas.html", "Datas"}, {"conceitos.html", "Conceitos"},
        {"problemas.html", "Problemas"}, {"referencias.html", "Referências"}
    };
    fputs("<!DOCTYPE html>\n<html lang=\"pt-br\">\n<head>\n"
          "  <meta charset=\"utf-8\">\n"
          "  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
          "  <title>", fp);
    fputs_html(fp, titulo);
    fputs(" - EP de MAC0122</title>\n"
          "  <link rel=\"stylesheet\" href=\"../estilo.css\">\n"
          "</head>\n<body>\n"
          "  <nav class=\"navbar\" aria-label=\"Navegação principal\">\n"
          "    <div class=\"navbar-conteudo\">\n"
          "      <a class=\"navbar-marca\" href=\"index.html\"><span class=\"chave\">{</span> MAC0122 <span class=\"chave\">}</span></a>\n"
          "      <input type=\"checkbox\" id=\"navbar-toggle\" class=\"navbar-toggle\">\n"
          "      <label for=\"navbar-toggle\" class=\"navbar-botao\" aria-label=\"Abrir menu\">&#9776;</label>\n"
          "      <ul class=\"navbar-menu\">\n", fp);
    for (size_t i = 0; i < sizeof(menu) / sizeof(menu[0]); i++) {
        fprintf(fp, "        <li><a href=\"%s\"%s>%s</a></li>\n",
                menu[i].arquivo,
                strcmp(menu[i].arquivo, pagina_atual) == 0 ? " aria-current=\"page\"" : "",
                menu[i].rotulo);
    }
    fputs("      </ul>\n    </div>\n  </nav>\n", fp);
}

static void escrever_rodape_html(FILE *fp) {
    fputs("  <footer class=\"rodape-site\">MAC0122 · Princípios de Desenvolvimento de Algoritmos</footer>\n"
          "</body>\n</html>\n", fp);
}

/* Usa os blocos separados por linhas vazias em fontes-txt/referencias.txt. */
static bool gerar_pagina_referencias(void) {
    FILE *entrada = fopen("fontes-txt/referencias.txt", "r");
    if (!entrada) {
        perror("fontes-txt/referencias.txt");
        return false;
    }
    FILE *fp = abrir_saida("referencias.html");
    if (!fp) {
        fclose(entrada);
        return false;
    }

    escrever_cabecalho_html(fp, "Referências", "referencias.html");
    fputs("  <main class=\"conteudo\">\n    <h1>Referências</h1>\n", fp);

    char linha[MAX_LINHA];
    bool introducao = true;
    bool bloco_com_texto = false;
    bool cartao_aberto = false;
    while (fgets(linha, sizeof(linha), entrada)) {
        limpar_fim(linha);
        const char *texto = pular_invisiveis(linha);
        if (*texto == '\0') {
            if (cartao_aberto) {
                fputs("    </article>\n", fp);
                cartao_aberto = false;
            }
            if (bloco_com_texto) introducao = false;
            bloco_com_texto = false;
            continue;
        }

        if (introducao) {
            fputs("    <p>", fp);
            fputs_html(fp, texto);
            fputs("</p>\n", fp);
        } else if (!cartao_aberto) {
            fputs("    <article class=\"cartao\">\n      <h2>", fp);
            fputs_html(fp, texto);
            fputs("</h2>\n", fp);
            cartao_aberto = true;
        } else {
            const char *prefixo = "Disponível em:";
            const char *url = NULL;
            if (strncmp(texto, prefixo, strlen(prefixo)) == 0)
                url = pular_invisiveis(texto + strlen(prefixo));
            if (url &&
                (strncmp(url, "https://", 8) == 0 || strncmp(url, "http://", 7) == 0)) {
                fputs("      <p>Disponível em: <a class=\"referencia-url\" href=\"", fp);
                fputs_html(fp, url);
                fputs("\" target=\"_blank\" rel=\"noopener\">", fp);
                fputs_html(fp, url);
                fputs("</a></p>\n", fp);
            } else {
                fputs("      <p>", fp);
                fputs_html(fp, texto);
                fputs("</p>\n", fp);
            }
        }
        bloco_com_texto = true;
    }
    if (cartao_aberto) fputs("    </article>\n", fp);
    fputs("  </main>\n", fp);
    escrever_rodape_html(fp);
    bool sucesso = !ferror(entrada) && fclose(fp) == 0;
    fclose(entrada);
    return sucesso;
}

/* Gera a página específica de um grupo (ex: tecnica_metodo_guloso.html). */
static bool gerar_pagina_grupo(const Taxonomia *tax, const GrupoIndice *grupo,
                              const AlgoritmoInfo *algs) {
    char nome_arquivo[640];
    snprintf(nome_arquivo, sizeof(nome_arquivo), "%s_%s.html", tax->prefixo_pagina, grupo->slug);

    FILE *fp = abrir_saida(nome_arquivo);
    if (!fp) {
        fprintf(stderr, "Erro ao criar '%s'\n", nome_arquivo);
        return false;
    }

    char titulo_pagina[600];
    snprintf(titulo_pagina, sizeof(titulo_pagina), "%s: %s", tax->nome_singular, grupo->titulo);
    escrever_cabecalho_html(fp, titulo_pagina, tax->arquivo_indice);

    fprintf(fp, "  <main class=\"conteudo\">\n"
                "    <a class=\"voltar\" href=\"%s\">&larr; Voltar para ", tax->arquivo_indice);
    fputs_html(fp, tax->nome_plural);
    fprintf(fp, "</a>\n    <h1>");
    fputs_html(fp, tax->nome_singular);
    fprintf(fp, ": ");
    fputs_html(fp, grupo->titulo);
    fprintf(fp, "</h1>\n");

    for (int i = 0; i < grupo->qtd_algoritmos; i++) {
        const AlgoritmoInfo *alg = &algs[grupo->indices_algoritmos[i]];
        fputs("    <article class=\"cartao\">\n      <h2><a href=\"", fp);
        fputs("../", fp);
        fputs_href(fp, alg->arquivo_html);
        fputs("\">", fp);
        fputs_html(fp, alg->nome);
        fputs("</a></h2>\n", fp);

        fprintf(fp, "      <p class=\"metadados\"><strong>Técnica:</strong> ");
        fputs_html(fp, alg->tecnica[0] ? alg->tecnica : "Não informada");
        if (alg->complexidade[0]) {
            fprintf(fp, " | <strong>Complexidade:</strong> ");
            fputs_html(fp, alg->complexidade);
        }
        fprintf(fp, " | <strong>Autores:</strong> ");
        fputs_html(fp, alg->autores[0] ? alg->autores : "Não informados");
        fprintf(fp, " | <strong>Data:</strong> ");
        fputs_html(fp, alg->data[0] ? alg->data : "-");
        fprintf(fp, "</p>\n");

        if (alg->problema[0] != '\0') {
            fprintf(fp, "      <p><strong>Problema:</strong> ");
            fputs_html(fp, alg->problema);
            fprintf(fp, "</p>\n");
        }
        fprintf(fp, "    </article>\n");
    }

    fputs("  </main>\n", fp);
    escrever_rodape_html(fp);
    return fclose(fp) == 0;
}

/* Gera a página índice da taxonomia (ex: tecnicas.html) */
static bool gerar_pagina_taxonomia(const Taxonomia *tax, const AlgoritmoInfo *algs) {
    FILE *fp = abrir_saida(tax->arquivo_indice);
    if (!fp) {
        fprintf(stderr, "Erro ao criar '%s'\n", tax->arquivo_indice);
        return false;
    }

    escrever_cabecalho_html(fp, tax->nome_plural, tax->arquivo_indice);
    fprintf(fp, "  <main class=\"conteudo\">\n    <h1>");
    fputs_html(fp, tax->nome_plural);
    fprintf(fp, "</h1>\n");
    if (strcmp(tax->arquivo_indice, "complexidades.html") == 0) {
        fputs("    <p>Tempo no pior caso da implementação apresentada. Para filas e pilhas, o custo é por operação; para a tabela hash, por busca ou inserção.</p>\n", fp);
    } else {
        fprintf(fp, "    <p>Explore os algoritmos por ");
        fputs_html(fp, tax->nome_singular);
        fprintf(fp, ".</p>\n");
    }
    fprintf(fp, "    <ul class=\"indice\">\n");

    bool sucesso = true;

    for (int i = 0; i < tax->qtd_grupos; i++) {
        const GrupoIndice *g = &tax->grupos[i];
        char link_grupo[640];
        snprintf(link_grupo, sizeof(link_grupo), "%s_%s.html", tax->prefixo_pagina, g->slug);

        fprintf(fp, "      <li><a href=\"%s\">", link_grupo);
        fputs_html(fp, g->titulo);
        fprintf(fp, "</a><span class=\"contagem\">%d %s</span></li>\n",
                g->qtd_algoritmos,
                g->qtd_algoritmos == 1 ? "algoritmo" : "algoritmos");

        if (!gerar_pagina_grupo(tax, g, algs)) sucesso = false;
    }

    fprintf(fp, "    </ul>\n  </main>\n");
    escrever_rodape_html(fp);
    return fclose(fp) == 0 && sucesso;
}

static int contar_paginas(const AlgoritmoInfo *algs, int qtd_algs, bool definicoes) {
    int total = 0;
    for (int i = 0; i < qtd_algs; i++) {
        if (algs[i].definicao == definicoes) total++;
    }
    return total;
}

static void escrever_lista_paginas(FILE *fp, const AlgoritmoInfo *algs,
                                   int qtd_algs, bool definicoes) {
    fputs("    <ul class=\"indice\">\n", fp);
    for (int i = 0; i < qtd_algs; i++) {
        if (algs[i].definicao != definicoes) continue;
        fputs("      <li><a href=\"", fp);
        fputs("../", fp);
        fputs_href(fp, algs[i].arquivo_html);
        fputs("\">", fp);
        fputs_html(fp, algs[i].nome);
        fputs("</a>", fp);
        if (!definicoes) {
            fputs("<span class=\"contagem\">", fp);
            fputs_html(fp, algs[i].tecnica[0] ? algs[i].tecnica : "Sem técnica");
            fputs("</span>", fp);
        }
        fputs("</li>\n", fp);
    }
    fputs("    </ul>\n", fp);
}

/* A capa antiga é a fonte visual do novo índice; assim não há duas cópias a manter. */
static bool copiar_capa(FILE *destino, FILE *capa_antiga) {
    char linha[4096];
    bool copiando = false;
    bool chamada_inserida = false;
    while (fgets(linha, sizeof(linha), capa_antiga)) {
        if (!copiando && strstr(linha, "<main class=\"capa\">") != NULL) copiando = true;
        if (!copiando) continue;
        if (!chamada_inserida && strstr(linha, "</section>") != NULL) {
            fputs("      <a class=\"capa-chamada\" href=\"#navegar\">Explorar algoritmos &darr;</a>\n", destino);
            chamada_inserida = true;
        }
        fputs(linha, destino);
        if (strstr(linha, "</main>") != NULL) return chamada_inserida;
    }
    return false;
}

/* Gera a página raiz com a capa original e links para todos os HTML filtrados. */
static bool gerar_pagina_principal(const Taxonomia *todas_tax, int qtd_tax,
                                  const AlgoritmoInfo *algs, int qtd_algs) {
    FILE *capa_antiga = fopen("index1.html", "r");
    if (!capa_antiga) {
        fprintf(stderr, "Erro ao abrir 'index1.html' (capa do site)\n");
        return false;
    }
    FILE *fp = abrir_saida("index.html");
    if (!fp) {
        fprintf(stderr, "Erro ao criar 'index.html'\n");
        fclose(capa_antiga);
        return false;
    }

    escrever_cabecalho_html(fp, "Início", "index.html");
    bool sucesso = copiar_capa(fp, capa_antiga);
    fclose(capa_antiga);
    if (!sucesso) fprintf(stderr, "Capa não encontrada em 'index1.html'\n");

    fputs("  <section id=\"navegar\" class=\"conteudo\">\n"
          "    <h1>Explore o acervo</h1>\n"
          "    <p>Consulte as páginas produzidas pelo filtro e navegue pelos índices.</p>\n"
          "    <div class=\"grade-indices\">\n", fp);
    for (int i = 0; i < qtd_tax; i++) {
        fprintf(fp, "      <a class=\"resumo-indice\" href=\"%s\"><h2>",
                todas_tax[i].arquivo_indice);
        fputs_html(fp, todas_tax[i].nome_plural);
        fprintf(fp, "</h2><p>%d %s</p></a>\n", todas_tax[i].qtd_grupos,
                todas_tax[i].qtd_grupos == 1 ? "categoria" : "categorias");
    }
    fprintf(fp, "      <a class=\"resumo-indice\" href=\"algoritmos.html\"><h2>Algoritmos</h2><p>%d páginas</p></a>\n",
            contar_paginas(algs, qtd_algs, false));
    fprintf(fp, "      <a class=\"resumo-indice\" href=\"definicoes.html\"><h2>Definições</h2><p>%d páginas</p></a>\n",
            contar_paginas(algs, qtd_algs, true));
    fputs("      <a class=\"resumo-indice\" href=\"referencias.html\"><h2>Referências</h2><p>Fontes consultadas</p></a>\n", fp);
    fputs("    </div>\n", fp);
    fprintf(fp, "    <h2>Algoritmos (%d)</h2>\n", contar_paginas(algs, qtd_algs, false));
    escrever_lista_paginas(fp, algs, qtd_algs, false);
    fprintf(fp, "    <h2>Definições (%d)</h2>\n", contar_paginas(algs, qtd_algs, true));
    escrever_lista_paginas(fp, algs, qtd_algs, true);
    fputs("  </section>\n", fp);
    escrever_rodape_html(fp);
    return fclose(fp) == 0 && sucesso;
}

static bool gerar_pagina_tipo(const AlgoritmoInfo *algs, int qtd_algs,
                              bool definicoes) {
    const char *arquivo = definicoes ? "definicoes.html" : "algoritmos.html";
    const char *titulo = definicoes ? "Definições" : "Algoritmos";
    FILE *fp = abrir_saida(arquivo);
    if (!fp) return false;
    escrever_cabecalho_html(fp, titulo, arquivo);
    fprintf(fp, "  <main class=\"conteudo\">\n"
                "    <h1>%s</h1>\n"
                "    <p>%d páginas disponíveis.</p>\n",
            titulo, contar_paginas(algs, qtd_algs, definicoes));
    escrever_lista_paginas(fp, algs, qtd_algs, definicoes);
    fputs("  </main>\n", fp);
    escrever_rodape_html(fp);
    return fclose(fp) == 0;
}

static bool gerar_pagina_grupo_problema(const GrupoIndice *grupo,
                                        const AlgoritmoInfo *algs) {
    char arquivo[640], titulo[600];
    snprintf(arquivo, sizeof(arquivo), "problema_%s.html", grupo->slug);
    snprintf(titulo, sizeof(titulo), "Problema: %s", grupo->titulo);
    FILE *fp = abrir_saida(arquivo);
    if (!fp) return false;

    escrever_cabecalho_html(fp, titulo, "problemas.html");
    fputs("  <main class=\"conteudo\">\n"
          "    <a class=\"voltar\" href=\"problemas.html\">&larr; Voltar para Problemas</a>\n"
          "    <h1>", fp);
    fputs_html(fp, grupo->titulo);
    fputs("</h1>\n", fp);

    for (int j = 0; j < grupo->qtd_algoritmos; j++) {
        const AlgoritmoInfo *alg = &algs[grupo->indices_algoritmos[j]];
        char palavra_chave[MAX_TEXTO];
        const char *descricao;
        separar_problema(alg->problema, palavra_chave, sizeof(palavra_chave), &descricao);

        fputs("    <article class=\"cartao cartao-problema\"><h2><a href=\"../", fp);
        fputs_href(fp, alg->arquivo_html);
        fputs("\">", fp);
        fputs_html(fp, palavra_chave);
        fputs("</a></h2><p class=\"enunciado-problema\">", fp);
        fputs_html(fp, descricao);
        fputs("</p><p class=\"metadados\">Texto: ", fp);
        fputs_html(fp, alg->nome);
        fputs("</p></article>\n", fp);
    }

    fputs("  </main>\n", fp);
    escrever_rodape_html(fp);
    return fclose(fp) == 0;
}

static bool gerar_pagina_problemas(const Taxonomia *problemas,
                                   const AlgoritmoInfo *algs) {
    FILE *fp = abrir_saida(problemas->arquivo_indice);
    if (!fp) return false;
    escrever_cabecalho_html(fp, "Problemas", problemas->arquivo_indice);
    fputs("  <main class=\"conteudo\">\n"
          "    <h1>Problemas</h1>\n"
          "    <p>Explore os textos pela palavra chave do problema.</p>\n"
          "    <ul class=\"indice\">\n", fp);

    bool sucesso = true;
    for (int i = 0; i < problemas->qtd_grupos; i++) {
        const GrupoIndice *grupo = &problemas->grupos[i];
        fprintf(fp, "      <li><a href=\"problema_%s.html\">", grupo->slug);
        fputs_html(fp, grupo->titulo);
        fprintf(fp, "</a><span class=\"contagem\">%d %s</span></li>\n",
                grupo->qtd_algoritmos,
                grupo->qtd_algoritmos == 1 ? "texto" : "textos");
        if (!gerar_pagina_grupo_problema(grupo, algs)) sucesso = false;
    }
    if (!problemas->qtd_grupos) {
        fputs("    </ul>\n    <p>Nenhum problema informado nas páginas.</p>\n", fp);
    } else {
        fputs("    </ul>\n", fp);
    }
    fputs("  </main>\n", fp);
    escrever_rodape_html(fp);
    return fclose(fp) == 0 && sucesso;
}

static void liberar_taxonomia(Taxonomia *tax) {
    for (int i = 0; i < tax->qtd_grupos; i++) {
        free(tax->grupos[i].indices_algoritmos);
    }
    free(tax->grupos);
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Uso: %s [diretorio-html-relativo-a-raiz]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *pasta_html = argc == 2 ? argv[1] : "processing/filter-output";
    DIR *dir = opendir(pasta_html);
    if (!dir) {
        perror(pasta_html);
        return EXIT_FAILURE;
    }

    AlgoritmoInfo *algoritmos = NULL;
    int qtd_algoritmos = 0;
    int capacidade = 0;
    struct dirent *item;
    while ((item = readdir(dir)) != NULL) {
        const char *ext = strrchr(item->d_name, '.');
        if (!ext || strcmp(ext, ".html") != 0) continue;
        if (strcmp(item->d_name, "referencias.txt.html") == 0) continue;

        char caminho[512];
        int n = snprintf(caminho, sizeof(caminho), "%s/%s", pasta_html, item->d_name);
        if (n < 0 || (size_t)n >= sizeof(caminho)) {
            fprintf(stderr, "Caminho longo demais: %s\n", item->d_name);
            continue;
        }
        if (qtd_algoritmos == capacidade) {
            capacidade = capacidade == 0 ? 16 : capacidade * 2;
            AlgoritmoInfo *novo = realloc(algoritmos, (size_t)capacidade * sizeof(*novo));
            if (!novo) {
                perror("Erro de memoria");
                free(algoritmos);
                closedir(dir);
                return EXIT_FAILURE;
            }
            algoritmos = novo;
        }
        if (ler_arquivo_html(caminho, item->d_name, &algoritmos[qtd_algoritmos])) qtd_algoritmos++;
    }
    closedir(dir);

    if (qtd_algoritmos == 0) {
        fprintf(stderr, "Nenhuma página HTML válida encontrada em '%s'.\n", pasta_html);
        free(algoritmos);
        return EXIT_FAILURE;
    }

#ifdef _WIN32
    int pasta_criada = _mkdir(PASTA_SAIDA);
#else
    int pasta_criada = mkdir(PASTA_SAIDA, 0755);
#endif
    if (pasta_criada != 0 && errno != EEXIST) {
        perror(PASTA_SAIDA);
        free(algoritmos);
        return EXIT_FAILURE;
    }
    DIR *saida = opendir(PASTA_SAIDA);
    if (!saida) {
        perror(PASTA_SAIDA);
        free(algoritmos);
        return EXIT_FAILURE;
    }
    closedir(saida);

    qsort(algoritmos, (size_t)qtd_algoritmos, sizeof(*algoritmos), comparar_algoritmos_direto);

    enum { TECNICAS, COMPLEXIDADES, AUTORES, DATAS, CONCEITOS, PROBLEMAS, QTD_TAXONOMIAS };
    Taxonomia taxonomias[QTD_TAXONOMIAS];
    inicializar_taxonomia(&taxonomias[TECNICAS], "Técnicas", "Técnica", "tecnicas.html", "tecnica");
    inicializar_taxonomia(&taxonomias[COMPLEXIDADES], "Complexidades", "Complexidade", "complexidades.html", "complexidade");
    inicializar_taxonomia(&taxonomias[AUTORES], "Autores", "Autor", "autores.html", "autor");
    inicializar_taxonomia(&taxonomias[DATAS], "Datas", "Data", "datas.html", "data");
    inicializar_taxonomia(&taxonomias[CONCEITOS], "Conceitos", "Conceito", "conceitos.html", "conceito");
    inicializar_taxonomia(&taxonomias[PROBLEMAS], "Problemas", "Problema", "problemas.html", "problema");

    for (int i = 0; i < qtd_algoritmos; i++) {
        indexar_campo_multiplo(&taxonomias[TECNICAS], algoritmos[i].tecnica, i);
        if (!algoritmos[i].definicao) {
            registrar_chave_taxonomia(&taxonomias[COMPLEXIDADES],
                                      algoritmos[i].complexidade[0] ? algoritmos[i].complexidade : "Não informada", i);
        }
        indexar_autores(&taxonomias[AUTORES], algoritmos[i].autores, i);
        if (algoritmos[i].data[0]) registrar_chave_taxonomia(&taxonomias[DATAS], algoritmos[i].data, i);
        indexar_campo_multiplo(&taxonomias[CONCEITOS], algoritmos[i].conceitos, i);
        if (algoritmos[i].problema[0]) {
            char palavra_chave[MAX_TEXTO];
            const char *descricao;
            separar_problema(algoritmos[i].problema, palavra_chave,
                             sizeof(palavra_chave), &descricao);
            registrar_chave_taxonomia(&taxonomias[PROBLEMAS], palavra_chave, i);
        }
    }

    bool sucesso = true;
    for (int t = 0; t < PROBLEMAS; t++) {
        ordenar_taxonomia(&taxonomias[t], algoritmos);
        if (!gerar_pagina_taxonomia(&taxonomias[t], algoritmos)) sucesso = false;
    }

    ordenar_taxonomia(&taxonomias[PROBLEMAS], algoritmos);
    qsort(taxonomias[PROBLEMAS].grupos, (size_t)taxonomias[PROBLEMAS].qtd_grupos,
          sizeof(GrupoIndice), comparar_grupos_slug);
    g_algoritmos_ref = algoritmos;
    for (int i = 0; i < taxonomias[PROBLEMAS].qtd_grupos; i++) {
        GrupoIndice *grupo = &taxonomias[PROBLEMAS].grupos[i];
        qsort(grupo->indices_algoritmos, (size_t)grupo->qtd_algoritmos,
              sizeof(int), comparar_indices_problema);
    }

    if (!gerar_pagina_tipo(algoritmos, qtd_algoritmos, false)) sucesso = false;
    if (!gerar_pagina_tipo(algoritmos, qtd_algoritmos, true)) sucesso = false;
    if (!gerar_pagina_problemas(&taxonomias[PROBLEMAS], algoritmos)) sucesso = false;
    if (!gerar_pagina_referencias()) sucesso = false;
    if (!gerar_pagina_principal(taxonomias, QTD_TAXONOMIAS, algoritmos, qtd_algoritmos)) sucesso = false;

    if (sucesso) {
        printf("Sucesso! %d páginas HTML indexadas de %s.\n", qtd_algoritmos, pasta_html);
        printf("Índices gerados em %s/: index.html, tecnicas.html, complexidades.html, algoritmos.html, definicoes.html, autores.html, datas.html, conceitos.html, problemas.html, referencias.html\n", PASTA_SAIDA);
    }

    for (int t = 0; t < QTD_TAXONOMIAS; t++) {
        liberar_taxonomia(&taxonomias[t]);
    }
    free(algoritmos);

    return sucesso ? EXIT_SUCCESS : EXIT_FAILURE;
}
