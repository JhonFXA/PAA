#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char placa[8];
    int pesoMax;
    int volumeMax;
} Veiculo;

typedef struct {
    char codigo[14];
    float valor;
    int peso;
    int volume;
    int usado;
} Pacote;

float max(float num1, float num2) {
    return (num1 >= num2) ? num1 : num2;
}

double totalValor = 0;
int totalPeso = 0;
int totalVolume = 0;

void removerPacotesUsados(Pacote pacotes[], int *quantPacote) {
    int novaQuantidade = 0;
    for (int i = 0; i < *quantPacote; i++) {
        if (!pacotes[i].usado) {
            pacotes[novaQuantidade++] = pacotes[i];
        }
    }
    *quantPacote = novaQuantidade;
}


void buscarSolucaoOtima(Veiculo veiculo, Pacote pacotes[], int quantPacote, FILE *saida) {

    float ***matriz = (float ***) malloc((quantPacote + 1) * sizeof(float **));

    for (int i = 0; i <= quantPacote; i++) {
        matriz[i] = (float **)malloc((veiculo.pesoMax + 1) * sizeof(float *));
        for (int w = 0; w <= veiculo.pesoMax; w++) {
            matriz[i][w] = (float *)malloc((veiculo.volumeMax + 1) * sizeof(float));
            for (int v = 0; v <= veiculo.volumeMax; v++) {
                matriz[i][w][v] = 0.0; 
            }
        }
    }

    int w, v;

    for (int i = 1; i <= quantPacote; i++) {
        for (w = 0; w <= veiculo.pesoMax; w++) {
            for (v = 0; v <= veiculo.volumeMax; v++) {
                if (w >= pacotes[i - 1].peso && v >= pacotes[i - 1].volume) {
                    matriz[i][w][v] = max(matriz[i - 1][w][v], matriz[i - 1][w - pacotes[i - 1].peso][v - pacotes[i - 1].volume] + pacotes[i - 1].valor);
                } else {
                    matriz[i][w][v] = matriz[i - 1][w][v];
                }
            }
        }
    }

    w = veiculo.pesoMax;
    v = veiculo.volumeMax;

    int pesoCarregado = veiculo.pesoMax;
    int volumeCarregado = veiculo.volumeMax;

    for (int i = quantPacote; i > 0; i--) {
        if (matriz[i][w][v] != matriz[i - 1][w][v]) {
            pacotes[i - 1].usado = 1; 
            totalValor -= pacotes[i-1].valor;
            totalPeso -= pacotes[i-1].peso;
            totalVolume -= pacotes[i-1].volume;
            w -= pacotes[i - 1].peso; 
            v -= pacotes[i - 1].volume;
        }
    }

    pesoCarregado -= w;
    volumeCarregado -= v;

    fprintf(saida,"[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->", veiculo.placa, matriz[quantPacote][veiculo.pesoMax][veiculo.volumeMax], pesoCarregado, (int)round((float)pesoCarregado / veiculo.pesoMax * 100), volumeCarregado, (int)round((float)volumeCarregado / veiculo.volumeMax * 100));

    w = veiculo.pesoMax;
    v = veiculo.volumeMax;
    int usados = 0; 
    for (int i = 0; i < quantPacote; i++) { 
        if (pacotes[i].usado) {
            if (usados > 0) {
                fprintf(saida,",");
            }
            fprintf(saida,"%s", pacotes[i].codigo);
            usados++;
        }
    }
    fprintf(saida,"\n");

    for (int i = 0; i <= quantPacote; i++) {
        for (int w = 0; w <= veiculo.pesoMax; w++) {
            free(matriz[i][w]);
        }
        free(matriz[i]);
    }
    free(matriz);
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

    int quantVeiculos, quantPacotes;
    fscanf(entrada, "%d", &quantVeiculos);
    
    Veiculo veiculos[quantVeiculos];

    for (int i = 0; i < quantVeiculos; i++) {
        fscanf(entrada, "%s %d %d", veiculos[i].placa, &veiculos[i].pesoMax, &veiculos[i].volumeMax);
    }

    fscanf(entrada, "%d", &quantPacotes);

    Pacote pacotes[quantPacotes];

    for (int i = 0; i < quantPacotes; i++) {
        fscanf(entrada, "%s %f %d %d", pacotes[i].codigo, &pacotes[i].valor, &pacotes[i].peso, &pacotes[i].volume);
        pacotes[i].usado = 0;
        totalValor += pacotes[i].valor;
        totalPeso += pacotes[i].peso;
        totalVolume += pacotes[i].volume;
    }

    int quantPacotesNaoUsados = quantPacotes;
    Pacote pacotesNaoUsados[quantPacotesNaoUsados];
    memcpy(pacotesNaoUsados, pacotes, quantPacotesNaoUsados * sizeof(Pacote));

    for(int i = 0; i < quantVeiculos; i++){
        buscarSolucaoOtima(veiculos[i], pacotesNaoUsados, quantPacotesNaoUsados, saida);
        removerPacotesUsados(pacotesNaoUsados, &quantPacotesNaoUsados);
    }

    fprintf(saida,"PENDENTE:R$%.2f,%dKG,%dL->", totalValor, totalPeso, totalVolume);
    for(int i = 0; i < quantPacotesNaoUsados; i++){
        fprintf(saida,"%s%s", i == 0? "": ",", pacotesNaoUsados[i].codigo);
    }
    fprintf(saida,"\n");
    return 0;
}
