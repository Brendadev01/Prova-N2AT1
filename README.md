# Prova-N2AT1
Prova individual da matéria de estrutura de dados - "Planta industrial inteligente"

Estes arquivos irão gerar timestamps aleatórios com os respectivos tipos que o usuário inserir no terminal. Em seguida, cada um será ordenado e salvo em arquivos diferentes, um para cada sensor. Por fim, será realizada a consulta do timestamp mais próximo da data inserida no terminal.

Compilação dos arquivos(no terminal):
gcc gerador.c -o gerador
gcc organizador.c -o organizador
gcc consulta.c -o consulta

Exemplos de execução:
1-  ./gerador 10 06 2025 12 06 2025 PRES int VIBR float UMID bool TEMP texto
2-  ./organizar timestamps
3-  ./consulta
