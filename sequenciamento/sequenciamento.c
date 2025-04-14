#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
    char codigo[15];
    char **genes;
    int32_t numGenes;
    int32_t porc;
} Doenca;


void calcular_tabela(int32_t* k, char* P) {
    int32_t n = strlen(P);
    k[0] = -1; 

    for (int32_t i = 1, j = -1; i < n; i++) {
        while (j >= 0 && P[j + 1] != P[i]) {
            j = k[j];
        }
        if (P[j + 1] == P[i]) {
            j++;
        }
        k[i] = j;
    }
}


void KMP(int32_t* k, int* count, char* T, char* P, int32_t tsub) {
    int32_t n = strlen(T), m = strlen(P);
    calcular_tabela(k, P);
    int32_t soma = 0;
    int32_t i = 0, j = -1;

    while (i < n) {
        while (j >= 0 && P[j + 1] != T[i]) {
            if (j < tsub - 1) {
                j = k[j];
                soma = j + 1;
            } else if (m - j - 1 >= tsub) {
                (*count) += soma;
                P = &P[j + 1];
                m = strlen(P);
                calcular_tabela(k, P);
                j = -1;
                soma = 0;
                continue;
            } else {
                (*count) += soma;
                return;
            }
        }
        if (P[j + 1] == T[i]) {
            j++;
            soma++;
        }
        if (j == m - 1) {
            (*count) += soma;
            return;
        }
        i++;
    }
}


void intercalar(Doenca* S, int32_t i, int32_t m, int32_t j) {
    int32_t i1 = i, i2 = m + 1;
    int32_t k = 0;
    Doenca* temp = malloc((j - i + 1) * sizeof(Doenca));
    
    if (!temp) {
        perror("Erro ao alocar memória temporária");
        exit(1);
    }

    while(i1 <= m && i2 <= j) {
        if (S[i1].porc >= S[i2].porc) temp[k++] = S[i1++];
        else temp[k++] = S[i2++];
    }

    while(i1 <= m) temp[k++] = S[i1++];
    while(i2 <= j) temp[k++] = S[i2++];

    memcpy(&S[i], temp, (j - i + 1) * sizeof(Doenca));
    free(temp);
}

void mergesort(Doenca* S, int32_t i, int32_t j) {
    if(i < j) {
        int32_t m = i + (j - i) / 2;
        mergesort(S, i, m);
        mergesort(S, m + 1, j);
        intercalar(S, i, m, j);
    }
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

    int tamanhoSubcadeia, numDoencas;
    fscanf(entrada, "%d", &tamanhoSubcadeia);
    
    char cadeia[50000];
    fscanf(entrada, "%s", cadeia);

    fscanf(entrada, "%d", &numDoencas);
    
    Doenca *doencas = malloc(numDoencas * sizeof(Doenca));
    if (!doencas) {
        perror("Erro ao alocar memória para doencas");
        fclose(entrada);
        fclose(saida);
        return 1;
    }


    int k[2000];
    for (int i = 0; i < numDoencas; i++) {
        fscanf(entrada, "%s %d", doencas[i].codigo, &doencas[i].numGenes);
        doencas[i].genes = malloc(doencas[i].numGenes * sizeof(char *));
        if (!doencas[i].genes) {
            perror("Erro ao alocar memória para genes");
            return 1;
        }

        int cont = 0;

        for (int j = 0; j < doencas[i].numGenes; j++) {
            char gene[2000];
            fscanf(entrada, "%s", gene);
            
            doencas[i].genes[j] = malloc(strlen(gene) + 1);
            strcpy(doencas[i].genes[j], gene);            
       
            int count = 0;
            KMP(k, &count, cadeia, doencas[i].genes[j], tamanhoSubcadeia);
            if (count >= (float)(strlen(doencas[i].genes[j]) * 0.9)) {
                cont++;
            }
        }
        doencas[i].porc = (cont * 100 + (doencas[i].numGenes / 2)) / doencas[i].numGenes;
    }


    mergesort(doencas, 0, numDoencas - 1);

    for (int i = 0; i < numDoencas; i++) {
        fprintf(saida, "%s->%d%%\n", doencas[i].codigo, doencas[i].porc);
    }

    free(doencas);
    fclose(entrada);
    fclose(saida);
    return 0;
}
