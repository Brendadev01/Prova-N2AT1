#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum {
    TIPO_INTEIRO,
    TIPO_BOOLEANO,
    TIPO_DECIMAL,
    TIPO_TEXTO,
    TIPO_INVALIDO
} TipoDado;

time_t converter_para_timestamp(int dia, int mes, int ano, int hora, int min, int seg) {
    struct tm t = {0};
    t.tm_year = ano - 1900;
    t.tm_mon = mes - 1;
    t.tm_mday = dia;
    t.tm_hour = hora;
    t.tm_min = min;
    t.tm_sec = seg;
    t.tm_isdst = -1;

    time_t ts = mktime(&t);
    if (ts == -1) {
        printf("Data inválida.\n");
        exit(1);
    }

    return ts;
}

TipoDado obterVariavel(char *tipo) {
    if (strcmp(tipo, "int") == 0) return TIPO_INTEIRO;
    if (strcmp(tipo, "bool") == 0) return TIPO_BOOLEANO;
    if (strcmp(tipo, "float") == 0) return TIPO_DECIMAL;
    if (strcmp(tipo, "texto") == 0) return TIPO_TEXTO;
    return TIPO_INVALIDO;
}

void gerarVariaveis(TipoDado tipo, char *saida) {
    switch (tipo) {
        case TIPO_INTEIRO:
            sprintf(saida, "%d", rand() % 1000);
            break;
        case TIPO_BOOLEANO:
            sprintf(saida, "%s", rand() % 2 ? "true" : "false");
            break;
        case TIPO_DECIMAL:
            sprintf(saida, "%.2f", (float)(rand() % 10000) / 100);
            break;
        case TIPO_TEXTO: {
            int len = 3 + rand() % 13;
            for (int i = 0; i < len; i++) {
                saida[i] = 'A' + (rand() % 26);
            }
            saida[len] = '\0';
            break;
        }
        default:
            strcpy(saida, "INV");
    }
}

int main(int argc, char *argv[]) {
    if (argc < 7 || (argc - 3) % 2 != 0) {
        printf("Uso: %s <dia_ini> <mes_ini> <ano_ini> <dia_fim> <mes_fim> <ano_fim> <sensor1> <tipo1> ...\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    int dia1 = atoi(argv[1]), mes1 = atoi(argv[2]), ano1 = atoi(argv[3]);
    int dia2 = atoi(argv[4]), mes2 = atoi(argv[5]), ano2 = atoi(argv[6]);

    time_t ts_ini = converter_para_timestamp(dia1, mes1, ano1, 0, 0, 0);
    time_t ts_fim = converter_para_timestamp(dia2, mes2, ano2, 23, 59, 59);

    FILE *out = fopen("timestamps.txt", "w");
    if (!out) {
        printf("Erro ao criar o arquivo.\n");
        return 1;
    }

    for (int i = 7; i < argc; i += 2) {
        char *sensor = argv[i];
        TipoDado tipo = obterVariavel(argv[i + 1]);

        if (tipo == TIPO_INVALIDO) {
            printf("Tipo inválido para sensor %s\n", sensor);
            continue;
        }

        for (int j = 0; j < 2000; j++) {
            time_t t = ts_ini + rand() % (ts_fim - ts_ini + 1);
            char valor[64];
            gerarVariaveis(tipo, valor);
            fprintf(out, "%ld %s %s\n", t, sensor, valor);
        }
    }

    fclose(out);
    printf("Arquivo 'timestamps.txt' gerado com sucesso.\n");
    return 0;
}