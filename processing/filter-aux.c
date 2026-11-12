#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>

#ifdef _WIN32
    #include <direct.h>
#else
    #include <sys/stat.h>
#endif

void escreve_char_html(int c, FILE *saida) {
    switch (c) {
        case '<': fputs("&#60;", saida); break;
        case '>': fputs("&#62;", saida); break;
        case '&': fputs("&#38;", saida); break;
        default:  fputc(c, saida); break;
    }
}

int palavra_reservada(const char *palavra, const char *vetor[]) {
    for (int i = 0; vetor[i] != NULL; i++) {
        if (strcmp(palavra, vetor[i]) == 0) {
            return 1;
        }
    }
    return 0; 
}