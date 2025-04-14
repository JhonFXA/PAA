#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int freq;
    char S;
    struct node *D;
    struct node *E;
} no;

typedef struct fila_p_min {
    int tam;
    int cap;
    no **V;
} fila_p_min;

const char hex_table[] = "0123456789ABCDEF";

char* tohex(int n) {
    static char hex[3];
    hex[0] = hex_table[(n / 16) & 0xF];
    hex[1] = hex_table[n & 0xF];
    hex[2] = '\0';
    return hex;
}

char* compactarRLE(char *string, int t, char *saida, int *tamanhoRLE) {
    int pos = 0;
    char aux[2] = {string[0], string[1]};
    int count = 1;

    for (int i = 2; i < t; i += 2) {
        if (string[i] == aux[0] && string[i + 1] == aux[1] && count < 255) {
            count++;
        } else {
            saida[pos++] = hex_table[(count / 16) & 0xF];
            saida[pos++] = hex_table[count & 0xF];
            saida[pos++] = aux[0];
            saida[pos++] = aux[1];
            aux[0] = string[i];
            aux[1] = string[i + 1];
            count = 1;
        }
    }

    saida[pos++] = hex_table[(count / 16) & 0xF];
    saida[pos++] = hex_table[count & 0xF];
    saida[pos++] = aux[0];
    saida[pos++] = aux[1];
    saida[pos] = '\0';

    *tamanhoRLE = pos;
    return saida;
}

fila_p_min *criar_fila_p_min(int cap) {
    fila_p_min *fpm = (fila_p_min*)malloc(sizeof(fila_p_min));
    fpm->tam = 0;
    fpm->cap = cap;
    fpm->V = (no**)malloc(cap * sizeof(no*));
    return fpm;
}

void heapify(fila_p_min *fpm, int idx) {
    int menor = idx;
    int esq, dir, menor_atual;
    no *temp;

    while (1) {
        esq = 2 * menor + 1;
        dir = 2 * menor + 2;
        menor_atual = menor;

        if (esq < fpm->tam && fpm->V[esq]->freq < fpm->V[menor_atual]->freq)
            menor_atual = esq;

        if (dir < fpm->tam && fpm->V[dir]->freq < fpm->V[menor_atual]->freq)
            menor_atual = dir;

        if (menor_atual != menor) {
            temp = fpm->V[menor_atual];
            fpm->V[menor_atual] = fpm->V[menor];
            fpm->V[menor] = temp;
            menor = menor_atual;
        } else {
            break;
        }
    }
}

void inserir(fila_p_min *fpm, int freq, char S, no *E, no *D) {
    no *novo_no = (no*)malloc(sizeof(no));
    novo_no->freq = freq;
    novo_no->S = S;
    novo_no->E = E;
    novo_no->D = D;

    int i = fpm->tam++;
    fpm->V[i] = novo_no;

    while (i && fpm->V[i]->freq < fpm->V[(i - 1) / 2]->freq) {
        no *temp = fpm->V[i];
        fpm->V[i] = fpm->V[(i - 1) / 2];
        fpm->V[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

no *extrair_min(fila_p_min *fpm) {
    no *temp = fpm->V[0];
    fpm->V[0] = fpm->V[fpm->tam - 1];
    fpm->tam--;
    heapify(fpm, 0);
    return temp;
}

no *construir_arvore(int H[], int n) {
    fila_p_min *fpm = criar_fila_p_min(n);
    for (int i = 0; i < n; i++)
        if (H[i]) inserir(fpm, H[i], i, NULL, NULL);

    while (fpm->tam > 1) {
        no *x = extrair_min(fpm);
        no *y = extrair_min(fpm);
        inserir(fpm, x->freq + y->freq, '\0', x, y);
    }
    return extrair_min(fpm);
}

char* bin_to_hex_str(char *bin, int len, char *hex) {
    int hex_len = (len + 3) / 4; // Cada 4 bits viram 1 dígito hexadecimal
    hex[hex_len] = '\0';

    for (int i = 0; i < hex_len; i++) {
        int value = 0;
        for (int j = 0; j < 4; j++) {
            int bit_index = len - 1 - (i * 4 + j);
            if (bit_index >= 0 && bin[bit_index] == '1') {
                value |= (1 << j);
            }
        }
        hex[hex_len - 1 - i] = hex_table[value];
    }

    return hex;
}

int hexToDecimal(char *hex) {
    int decimal = 0;

    for (int i = 0; i < 2; i++) {
        decimal *= 16;
        if (hex[i] >= '0' && hex[i] <= '9') {
            decimal += hex[i] - '0';
        } else if (hex[i] >= 'A' && hex[i] <= 'F') {
            decimal += hex[i] - 'A' + 10;
        } else if (hex[i] >= 'a' && hex[i] <= 'f') {
            decimal += hex[i] - 'a' + 10;
        }
    }

    return decimal;
}

void gerar_codigos(no *raiz, char codigos[256][256], int *tamanhos, char *codigo, int profundidade) {
    if (raiz->E) {
        codigo[profundidade] = '0';
        gerar_codigos(raiz->E, codigos, tamanhos, codigo, profundidade + 1);
    }
    if (raiz->D) {
        codigo[profundidade] = '1';
        gerar_codigos(raiz->D, codigos, tamanhos, codigo, profundidade + 1);
    }
    if (!raiz->E && !raiz->D) {
        codigo[profundidade] = '\0';
        tamanhos[(unsigned char)raiz->S] = profundidade;
        for (int i = 0; i <= profundidade; i++) {
            codigos[(unsigned char)raiz->S][i] = codigo[i];
        }
    }
}

char* compactarHUF(char *string, int t, int freq[], char *saida, int *tamanhoHUF) {
    no *raiz = construir_arvore(freq, 256);
    char codigos[256][256] = {{0}};
    int tamanhos[256] = {0};
    char codigo[256];
    gerar_codigos(raiz, codigos, tamanhos, codigo, 0);

    int pos = 0;

    for (int i = 0; i < t; i += 2) {
        char hex[3] = {string[i], string[i + 1], '\0'};
        int decimal = hexToDecimal(hex);
        char *codigo_atual = codigos[decimal];
        int len = tamanhos[decimal];
        for (int j = 0; j < len; j++) {
            saida[pos++] = codigo_atual[j];
        }
    }

    int len = pos;
    int padding = 8 - (len % 8);
    if (padding < 8) {
        for (int i = 0; i < padding; i++) {
            saida[pos++] = '0';
        }
    }
    saida[pos] = '\0';

    static char hex_buffer[20000]; // Buffer fixo para armazenar a saída hexadecimal
    char *saida_hex = bin_to_hex_str(saida, pos, hex_buffer);
    *tamanhoHUF = pos / 4; // Cada 4 bits viram 1 dígito hexadecimal
    return saida_hex;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada.txt saida.txt\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *saida = fopen(argv[2], "w");
    if (!saida) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(entrada);
        return 1;
    }

    int n;
    fscanf(entrada, "%d", &n); // Lê o número de sequências

    char saida_HUF[80000];
    char saida_RLE[40000];

    for (int i = 0; i < n; i++) {
        int tam;
        fscanf(entrada, "%d", &tam);

        char sequencia[tam * 2];
        int freq[256] = {0};

        for (int j = 0; j < tam; j++) {
            char aux[3];
            fscanf(entrada, "%s", aux);
            sequencia[j*2] = aux[0];
            sequencia[j*2+1] = aux[1];
            char hex[3] = {aux[0], aux[1], '\0'};
            freq[hexToDecimal(hex)]++;
        }

        int tamanhoRLE, tamanhoHUF;
        char *RLE = compactarRLE(sequencia, tam * 2, saida_RLE, &tamanhoRLE);
        float taxaRLE = ((float)tamanhoRLE / (2 * tam)) * 100;
        char *HUF = compactarHUF(sequencia, tam * 2, freq, saida_HUF, &tamanhoHUF);
        float taxaHUF = ((float)tamanhoHUF / (2 * tam)) * 100;

        if (taxaHUF < taxaRLE) {
            fprintf(saida, "%d->HUF(%.2f%%)=%s\n", i, taxaHUF, HUF);
        } else if (taxaHUF > taxaRLE) {
            fprintf(saida, "%d->RLE(%.2f%%)=%s\n", i, taxaRLE, RLE);
        } else {
            fprintf(saida, "%d->HUF(%.2f%%)=%s\n", i, taxaHUF, HUF);
            fprintf(saida, "%d->RLE(%.2f%%)=%s\n", i, taxaRLE, RLE);
        }
    }

    fclose(entrada);
    fclose(saida);

    return 0;
}