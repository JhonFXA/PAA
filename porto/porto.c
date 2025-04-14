#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//Solução utilizando Tabela Hash

typedef struct container {
    char codigo[12];
    char cnpj[20];
    uint32_t peso;
} Container;

typedef struct divergencia {
    Container contCadastrado;
    Container contSelecionado;
    float percentual;
} Divergencia;

void inicializarTabela(Container tabela[], uint32_t tamanho){
    for(uint32_t i = 0; i < tamanho; i++){
        strcpy(tabela[i].codigo, "0");
    }
}

void copiar(Divergencia *destino, Divergencia *origem, int32_t n){
    for(int32_t i = 0; i < n; i++){
        destino[i] = origem[i];
    }
}

void intercalar(Divergencia* S, Divergencia* E, int32_t i, int32_t m, int32_t j) {
    int32_t i1 = i, i2 = m + 1, k = i;
    while(i1 <= m && i2 <= j) {
        if(E[i1].percentual >= E[i2].percentual) S[k++] = E[i1++];
        else S[k++] = E[i2++];
    }
    if(i1 > m) copiar(&S[k], &E[i2], j - i2 + 1);
    else copiar(&S[k], &E[i1], m - i1 + 1);
    copiar(&E[i], &S[i], j - i + 1);
}

void mergesort(Divergencia* S, Divergencia* E, int32_t i, int32_t j) {
    if(i < j) {
        int32_t m = i + (j - i) / 2;
        mergesort(S, E, i, m);
        mergesort(S, E, m + 1, j);
        intercalar(S, E, i, m, j);
    }
}


uint32_t hash(char codigo[], uint32_t tamanhoHash) {
    uint32_t hashValue = 0;
    for (uint32_t i = 0; i < strlen(codigo); i++) {
        hashValue = (hashValue * 31 + codigo[i]) % tamanhoHash;
    }
    return hashValue;
}


void adicionarNaTabela(Container tabela[], Container cont, uint32_t tamanho){
    uint32_t posicao = hash(cont.codigo, tamanho);
    while(strcmp(tabela[posicao].codigo, "0") != 0){
        posicao = (posicao + 1) % tamanho;
    }
    tabela[posicao] = cont;
}

void encontrarDivergencia(Container *cont, uint32_t tamanhoCadastrados, uint32_t tamanhoSelecionados, Container tabelaHash[], FILE *saida) {
    Divergencia divergenciasPeso[tamanhoSelecionados];
    int32_t l = 0;
    for(uint32_t i = 0; i < tamanhoCadastrados; i++){
        uint32_t posicao = hash(cont[i].codigo, tamanhoSelecionados * 2);
        while(strcmp(tabelaHash[posicao].codigo, "0")!=0){
            if(strcmp(tabelaHash[posicao].codigo, cont[i].codigo) == 0){
                float percentual = round((abs(cont[i].peso - tabelaHash[posicao].peso) * 100.0) / cont[i].peso);
                if(strcmp(tabelaHash[posicao].cnpj, cont[i].cnpj) != 0){
                    fprintf(saida, "%s:%s<->%s\n", cont[i].codigo, cont[i].cnpj, tabelaHash[posicao].cnpj);
                }
                else if(percentual > 10){
                    Divergencia divergencia = {cont[i], tabelaHash[posicao], percentual};
                    divergenciasPeso[l++] = divergencia;
                }
                break;
            }
            posicao = (posicao + 1) % (tamanhoSelecionados * 2);
        }
    }
    Divergencia *auxPeso = (Divergencia *)malloc(l * sizeof(Divergencia));

    mergesort(auxPeso, divergenciasPeso, 0, l - 1);

    for(int32_t i = 0; i < l; i++){
        fprintf(saida, "%s:%dkg(%.0f%%)\n", divergenciasPeso[i].contCadastrado.codigo, abs(divergenciasPeso[i].contCadastrado.peso - divergenciasPeso[i].contSelecionado.peso), divergenciasPeso[i].percentual);
    }
    free(auxPeso);
}

int main(int argc, char *argv[]) {
    if(argc != 3){
        fprintf(stderr, "Uso: %s entrada.txt saida.txt\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");

    if(entrada == NULL){
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    FILE *saida = fopen(argv[2], "w");
    if(saida == NULL){
        perror("Erro ao abrir o arquivo de saída");
        return 1;
    }

    uint32_t tamanhoCadastrados = 0, tamanhoSelecionados = 0;

    fscanf(entrada, "%u", &tamanhoCadastrados);

    Container contCadastrados[tamanhoCadastrados];
    for(uint32_t i = 0; i < tamanhoCadastrados; i++){
        fscanf(entrada, "%s %s %u", contCadastrados[i].codigo, contCadastrados[i].cnpj, &contCadastrados[i].peso);
    }

    fscanf(entrada, "%u", &tamanhoSelecionados);

    uint32_t tamanhoHash = tamanhoSelecionados * 2;
    Container tabelaHash[tamanhoHash];
    inicializarTabela(tabelaHash, tamanhoHash);

    for(uint32_t i = 0; i < tamanhoSelecionados; i++){
        Container contSelecionado;
        fscanf(entrada, "%s %s %u", contSelecionado.codigo, contSelecionado.cnpj, &contSelecionado.peso);
        adicionarNaTabela(tabelaHash, contSelecionado, tamanhoHash);
    }
    fclose(entrada);


    encontrarDivergencia(contCadastrados, tamanhoCadastrados, tamanhoSelecionados, tabelaHash, saida);
    fclose(saida);

    return 0;
}