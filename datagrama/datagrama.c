#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int codigo;
    int quantidade;
    char **dados;
} Pacote;

int main(int argc, char *argv[]){
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

    int n, m, t = 0, p = 0;
    fscanf(entrada, "%d %d", &n, &m);

    Pacote *pacotes = (Pacote *)malloc(n*sizeof(Pacote));

    for(int i = 0; i < n; i++){
        pacotes[i].codigo = -1;
    }

    for(int i = 0; i < n/m ;i++){
        for(int j =0; j < m; j++){
            Pacote pacote;
            fscanf(entrada, "%d %d", &pacote.codigo, &pacote.quantidade);
            pacote.dados = (char **)malloc(pacote.quantidade * sizeof(char *));
            for (int k = 0; k < pacote.quantidade; k++) {
                pacote.dados[k] = (char *)malloc(3 * sizeof(char));
                fscanf(entrada, "%2s", pacote.dados[k]);
            }
            pacotes[pacote.codigo] = pacote;
            p++;
        }

        if(pacotes[t].codigo != -1){
            fprintf(saida, "|");
            while(t < p){
                if(pacotes[t].codigo == -1){
                    break;
                } else {
                    for (int b = 0; b < pacotes[t].quantidade; b++) {
                        fprintf(saida, "%s%s", pacotes[t].dados[b], b + 1 == pacotes[t].quantidade ? "" : ",");
                    }
                    fprintf(saida, "|");
                    t++;
                }
            }
            fprintf(saida, "\n");
        }
    }

    if(n%m != 0){
        for(int i = 0; i < n % m; i++){
            Pacote pacote;
            fscanf(entrada, "%d %d", &pacote.codigo, &pacote.quantidade);
            pacote.dados = (char **)malloc(pacote.quantidade * sizeof(char *));
            for (int k = 0; k < pacote.quantidade; k++) {
                pacote.dados[k] = (char *)malloc(3 * sizeof(char));
                fscanf(entrada, "%2s", pacote.dados[k]);
            }
            pacotes[pacote.codigo] = pacote;
            p++;
        }
        while(t < p){
           fprintf(saida, "|");
            for (int b = 0; b < pacotes[t].quantidade; b++) {
                fprintf(saida, "%s%s", pacotes[t].dados[b], b + 1 == pacotes[t].quantidade ? "" : ",");
            }
            t++; 
        }
    
        fprintf(saida, "|\n");
    }


    return 0;
}