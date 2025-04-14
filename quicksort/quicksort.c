#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char type[3];
    int steps;
} R;

void swap(int *a, int *b, int *cont){
    (*cont)++;
    int aux = *a;
    *a = *b;
    *b = aux;
}

//=================================  H  =======================================

int H(int *V, int i, int j, int *cont){
    int P = V[i], x = i - 1, y = j + 1;
    while(1){
        while(V[--y] > P);
        while(V[++x] < P);
        if(x < y) swap(&V[x], &V[y], cont);
        else return y;
    }
}

int calcMed(int i, int j, int *V){
    int a = i+(j-i+1)/4, b = i+(j-i+1)/2, c = i+(j-i+1)*3/4;
    int value1 = V[a];
    int value2 = V[b]; 
    int value3 = V[c];

    if((value1 == value3 || value1 == value2) || (value1<value2 && value1>value3)){
        return a;
    }
    if((value2 == value3) || (value1<value2 && value2<value3)){
        return b;
    }
    if (value1<value2){
        return c;
    }
    if(value2>value3){
        return b;
    }
    if (value1<value3){
        return a;
    } 
    else return c;
}

int HM(int *V, int i, int j, int *cont){
    int indice = calcMed(i, j, V);
    swap(&V[i], &V[indice], cont);
    int P = V[i], x = i - 1, y = j + 1;
    while(1){
        while(V[--y] > P);
        while(V[++x] < P);
        if(x < y) swap(&V[x], &V[y], cont);
        else return y;
    }
}

int H_R(int *V, int i, int j, int *cont){
    swap(&V[i], &V[i + (abs(V[i]) % (j - i + 1))], cont);
    return H(V, i, j, cont);
}

void quicksortH(int *V,  int i, int j, int *cont){
    (*cont) += 1;
    if(i < j){
        int pivo = H(V, i, j, cont);
        quicksortH(V, i, pivo, cont);
        quicksortH(V, pivo + 1, j, cont);
    }
}


void quicksortHM(int *V,  int i, int j, int *cont){
    (*cont)++;
    if(i < j){
        int pivo = HM(V, i, j, cont);
        quicksortHM(V, i, pivo, cont);
        quicksortHM(V, pivo + 1, j, cont);
    }
}

void quicksortHR(int *V,  int i, int j, int *cont){
    (*cont)++;
    if(i < j){
        int pivo = H_R(V, i, j, cont);
        quicksortHR(V, i, pivo, cont);
        quicksortHR(V, pivo + 1, j, cont);
    }
}


int L(int *V, int i, int j, int *cont){
    int P = V[j], x = i - 1, y = i;
    for(y = i; y < j; y++){
        if(V[y] <= P) swap(&V[++x], &V[y], cont);
    }
    swap(&V[++x], &V[j], cont);
    return x;
}

int LM(int *V, int i, int j, int *cont){
    int indice = calcMed(i, j, V);
    swap(&V[j], &V[indice], cont);
    int P = V[j], x = i - 1, y = i;
    for(y = i; y < j; y++){
        if(V[y] <= P) swap(&V[++x], &V[y], cont);
    }
    swap(&V[++x], &V[j], cont);
    return x;
}

int L_R(int *V, int i, int j, int *cont){
    swap(&V[j], &V[i + (abs(V[i]) % (j - i + 1))], cont);
    return L(V, i, j, cont);
}


void quicksortL(int *V,  int i, int j, int *cont){
    (*cont)++;
    if(i < j){
        int pivo = L(V, i, j, cont);
        quicksortL(V, i, pivo - 1, cont);
        quicksortL(V, pivo + 1, j, cont);
    }
}

void quicksortLM(int *V,  int i, int j, int *cont){
    (*cont)++;
    if(i < j){
        int pivo = LM(V, i, j, cont);
        quicksortLM(V, i, pivo - 1, cont);
        quicksortLM(V, pivo + 1, j, cont);
    }
}

void quicksortLR(int *V,  int i, int j, int *cont){
    (*cont)++;
    if(i < j){
        int pivo = L_R(V, i, j, cont);
        quicksortLR(V, i, pivo - 1, cont);
        quicksortLR(V, pivo + 1, j, cont);
    }
}


void copy(R *destiny, R *origin, int n){
    for(int i = 0; i < n; i++){
        destiny[i] = origin[i];
    }
}


void intercalate(R* S, R* E, int i, int m, int j) {
    int i1 = i, i2 = m + 1, k = i;
    while(i1 <= m && i2 <= j) {
        if(E[i1].steps <= E[i2].steps) S[k++] = E[i1++];
        else S[k++] = E[i2++];
    }
    if(i1 > m) copy(&S[k], &E[i2], j - i2 + 1);
    else copy(&S[k], &E[i1], m - i1 + 1);
    copy(&E[i], &S[i], j - i + 1);
}


void mergesort(R* S, R* E, int i, int j) {
    if(i < j) {
        int m = i + (j - i) / 2;
        mergesort(S, E, i, m);
        mergesort(S, E, m + 1, j);
        intercalate(S, E, i, m, j);
    }
}


void copyNum(int *destiny ,int *origin, int n){
    for(int i = 0; i < n; i++){
        destiny[i] = origin[i];
    }
}



int main(int argc, char *argv[]){
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

    int n = 0;

    fscanf(entrada, "%d", &n);

    for(int i = 0; i < n; i++){
        int size = 0;
        fscanf(entrada, "%d", &size);
        int *vector = (int *)malloc(size * sizeof(int));
        for (int i = 0; i < size; i++) {
            fscanf(entrada, "%d", &vector[i]);
        }

        R result[] = {{"LP", 0}, {"LM", 0}, {"LA", 0}, {"HP", 0}, {"HM", 0}, {"HA", 0}};
        R *aux = (R *)malloc(6 * sizeof(R));
        int *copy = (int *)malloc(size * sizeof(int));
        copyNum(copy, vector, size);

        quicksortL(copy, 0, size - 1, &result[0].steps);
        copyNum(copy, vector, size);

        quicksortLM(copy, 0, size - 1, &result[1].steps);
        copyNum(copy, vector, size);
        
        quicksortLR(copy, 0, size - 1, &result[2].steps);
        copyNum(copy, vector, size);
        
        quicksortH(copy, 0, size - 1, &result[3].steps);
        copyNum(copy, vector, size);
        
        quicksortHM(copy, 0, size - 1, &result[4].steps);
        copyNum(copy, vector, size);
        
        quicksortHR(copy, 0, size - 1, &result[5].steps);

        mergesort(aux, result, 0, 5);

        fprintf(saida,"%d:N(%d)", i, size);
        for(int j = 0; j < 6; j++){
            fprintf(saida,",%s(%d)", result[j].type, result[j].steps);
        }
        free(aux);
        free(copy);
        free(vector);
        fprintf(saida,"\n");
    }
    fclose(entrada);
    fclose(saida);
}
