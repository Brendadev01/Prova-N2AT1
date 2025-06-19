#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_SENSORES 100
#define TAM_INICIAL 20

typedef enum {
    INTEIRO,
    BOOLEANO,
    DECIMAL,
    TEXTO
} Categoria;


typedef struct {
    long tempo;
    char nome_sensor[50];
    void* info;
    Categoria tipo;
} Dado;



typedef struct {
    char identificador[50];
    Dado** registros;
    int usados;
    int capacidade;
} Coletor;



Dado* fazerDados(long tempo, const char* sensor, const char* valor);
void ordenar(Dado** dados, int n);
void liberaStructColetor(Coletor* c);
void* interpretar(const char* entrada, Categoria tipo);
Categoria classificar(const char* entrada);
void salvaArquivo(Coletor* c);

void iniciar(Coletor* c, const char* id) {
    strcpy(c->identificador, id);
    c->usados = 0;
    c->capacidade = TAM_INICIAL;
    c->registros = (Dado**)malloc(c->capacidade * sizeof(Dado*));
}

void inserir(Coletor* c, Dado* d) {
    if (c->usados == c->capacidade) {
        c->capacidade *= 2;
        c->registros = (Dado**)realloc(c->registros, c->capacidade * sizeof(Dado*));
    }
    c->registros[c->usados++] = d;
}

int buscar_sensor(Coletor** coletores, int total, const char* id) {
    for (int i = 0; i < total; i++) {
        if (strcmp(coletores[i]->identificador, id) == 0)
            return i;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Modo de uso: %s <entrada.txt>\n", argv[0]);
        return 1;
    }

    FILE* entrada = fopen(argv[1], "r");
    if (!entrada) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    Coletor* sensores[MAX_SENSORES];
    int qtd_sensores = 0;
    char linhas[256];

    while (fgets(linhas, sizeof(linhas), entrada)) {
        long tempo;
        char id[50], valor[100];




        if (sscanf(linhas, "%ld %49s %99s", &tempo, id, valor) != 3) {
            printf("Linhas ignoradas: %s", linhas);
            continue;
        }

        Dado* d = fazerDados(tempo, id, valor);
        if (!d) continue;

        int indice = buscar_sensor(sensores, qtd_sensores, id);
        if (indice == -1) {
            if (qtd_sensores >= MAX_SENSORES) continue;
            sensores[qtd_sensores] = (Coletor*)malloc(sizeof(Coletor));
            iniciar(sensores[qtd_sensores], id);
            indice = qtd_sensores++;
        }

        inserir(sensores[indice], d);
    }

    fclose(entrada);

    for (int i = 0; i < qtd_sensores; i++) {
        ordenar(sensores[i]->registros, sensores[i]->usados);
        salvaArquivo(sensores[i]);
        liberaStructColetor(sensores[i]);
        free(sensores[i]);
    }

    return 0;
}




Categoria classificar(const char* entrada) {
    if (strcmp(entrada, "true") == 0 || strcmp(entrada, "false") == 0) return BOOLEANO;

    char* fim;
    strtol(entrada, &fim, 10);
    if (*fim == '\0') return INTEIRO;

    strtof(entrada, &fim);
    if (*fim == '\0') return DECIMAL;

    return TEXTO;
}

void* interpretar(const char* entrada, Categoria tipo) {
    switch (tipo) {
        case INTEIRO: {
            int* val = malloc(sizeof(int));
            if (val) *val = atoi(entrada);
            return val;
        }
        
        case BOOLEANO: {
            bool* val = malloc(sizeof(bool));
            if (val) *val = strcmp(entrada, "true") == 0;
            return val;
        }

        case DECIMAL: {
            float* val = malloc(sizeof(float));
            if (val) *val = atof(entrada);
            return val;
        }

        case TEXTO: {
            char* val = malloc(strlen(entrada) + 1);
            if (val) strcpy(val, entrada);
            return val;
        }
    }
    return NULL;
}

Dado* fazerDados(long tempo, const char* sensor, const char* valor) {
    Categoria tipo = classificar(valor);
    void* info = interpretar(valor, tipo);
    if (!info) return NULL;

    Dado* d = malloc(sizeof(Dado));
    d->tempo = tempo;
    d->tipo = tipo;
    strcpy(d->nome_sensor, sensor);
    d->info = info;
    return d;
}

void ordenar(Dado** dados, int n) {
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
            if (dados[j]->tempo > dados[j + 1]->tempo) {
                Dado* tmp = dados[j];
                dados[j] = dados[j + 1];
                dados[j + 1] = tmp;
            }
}

void salvaArquivo(Coletor* c) {
    char nome_arq[100];
    snprintf(nome_arq, sizeof(nome_arq), "%s.txt", c->identificador);

    FILE* out = fopen(nome_arq, "w");
    if (!out) {
        printf("Erro ao salvar: %s\n", nome_arq);
        return;
    }

    for (int i = 0; i < c->usados; i++) {
        Dado* d = c->registros[i];
        fprintf(out, "%ld %s ", d->tempo, d->nome_sensor);
        switch (d->tipo) {
            case INTEIRO: fprintf(out, "%d\n", *((int*)d->info)); break;

            case BOOLEANO: fprintf(out, "%s\n", *((bool*)d->info) ? "true" : "false"); break;

            case DECIMAL: fprintf(out, "%.2f\n", *((float*)d->info)); break;
            
            case TEXTO: fprintf(out, "%s\n", (char*)d->info); break;
        }
    }

    fclose(out);
    printf("Arquivo criado: %s (%d registros)\n", nome_arq, c->usados);
}

void liberaStructColetor(Coletor* c) {
    for (int i = 0; i < c->usados; i++) {
        free(c->registros[i]->info);
        free(c->registros[i]);
    }
    free(c->registros);
}
