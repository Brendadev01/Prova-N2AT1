#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    long timestamp;
    char valor[64];
} Registro;

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

long buscaRegistros(Registro registros[], int n, long alvo) {
    int inicio = 0, fim = n - 1;
    int mais_prox = -1;
    long menor_dif = 999999999;

    while (inicio <= fim) {
        int meio = (inicio + fim) / 2;
        long diff = labs(registros[meio].timestamp - alvo);

        if (diff < menor_dif) {
            menor_dif = diff;
            mais_prox = meio;
        }

        if (registros[meio].timestamp < alvo)
            inicio = meio + 1;
        else
            fim = meio - 1;
    }

    return mais_prox;
}

int main() {
    char nome_sensor[32];
    int dia, mes, ano, hora, min, seg;

    printf("Digite o nome do sensor: ");
    scanf("%s", nome_sensor);

    printf("Digite a data e hora (dd mm aaaa hh mm ss): ");
    if (scanf("%d %d %d %d %d %d", &dia, &mes, &ano, &hora, &min, &seg) != 6) {
        printf("Entrada inválida!\n");
        return 1;
    }

    time_t alvo = converter_para_timestamp(dia, mes, ano, hora, min, seg);

    char nome_arquivo[64];
    sprintf(nome_arquivo, "%s.txt", nome_sensor);

    FILE *arq = fopen(nome_arquivo, "r");
    if (!arq) {
        printf("Sensor '%s' não encontrado.\n", nome_sensor);
        return 1;
    }

    Registro registros[10000];
    int total = 0;

    while (fscanf(arq, "%ld %*s %s", &registros[total].timestamp, registros[total].valor) == 2) {
        total++;
    }

    fclose(arq);

    if (total == 0) {
        printf("Nenhuma leitura encontrada.\n");
        return 1;
    }

    int indice = buscaaproximada(registros, total, alvo);
    printf("Leitura mais próxima:\nTimestamp: %ld\nValor: %s\n", registros[indice].timestamp, registros[indice].valor);

    return 0;
}
