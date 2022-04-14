#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//compilar usando gcc hamming.c -o hamming -lm

/*   Encontra o tamanho em bytes do arquivo   */
long int findSize(FILE * f) {

    fseek(f, 0, SEEK_END); // Move o ponteiro para o final do arquivo

    long int size = ftell(f); // Obtem a posicao atual do ponteiro do arquivo

    if (size != -1)
        printf("Tamanho do arquivo: %ld bytes\n", size);
    else
        printf("Deu algum ERRO.\n");

    fseek(f, 0, SEEK_SET); // Move o ponteiro para o inicio do arquivo

    return size;
}

/*   Transforma o numero decimal em binário   */
int * decimalToBinary(int x) {

    int * binaryNumber = (int * ) calloc(8, sizeof(int)); // Array para armazenar o numero binario (string/palavra binaria)

    int i = 0, j = 0, k = 0;

    while (x > 0) {
        binaryNumber[j] = x % 2;
        x = x / 2;
        j++;
    }
    return binaryNumber;
}

int binaryToDecimal(int * x, int numbers) {
    int decimalNumber = 0; // Variavel que armazena o valor decimal da palavra binaria

    int i = 0;
    for (i = 0; i < numbers; i++) {
        if (x[i] != 0)
            decimalNumber = decimalNumber + pow(2, i);
    }

    return decimalNumber;
}

int main(int argc, char * argv[]) {

    FILE * f;
    long int size = 0;
    f = fopen(argv[1], "r+b");

    if (f == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }

    size = findSize(f);
    char temp;
    char * word;
    int aux = 0;
    int C[4];
    int novoC[4];
    int sindrome[4];
    int G, novoG;
    int pos;
    int valor;

    word = (char * ) malloc(size * sizeof(char));

    while ((temp = fgetc(f)) != EOF) {
        word[aux] = temp;
        aux++;
    }
    /* for(int i = 0;i<size;i++){
         printf("%c",word[i]);
     }*/

    int i, * byte, * msgBits;
    byte = (int * ) calloc(8, sizeof(int));

    printf("\n---------------------------------------------\n");
    fclose(f);
    remove(argv[1]);
    system("read -p 'Arquivo excluído, pressione ENTER' var");
    f = fopen(argv[2], "r+b");
    if (f == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 0;
    }
    for (int x = 0; x < size; x++) {
        byte = decimalToBinary((int) word[x]);
        printf("\nbyte %d\n", x);

        for (i = 0; i < 8; i++) {
            printf("%d ", byte[i]);
        }
        msgBits = (int * ) calloc(14, sizeof(int));
        msgBits[13] = byte[0]; //M8
        msgBits[12] = byte[1]; //M7
        msgBits[11] = byte[2]; //M6
        msgBits[10] = byte[3]; //M5
        msgBits[8] = byte[4]; //M4
        msgBits[7] = byte[5]; //M3
        msgBits[6] = byte[6]; //M2
        msgBits[4] = byte[7]; //M1

        C[0] = msgBits[2] = byte[7] ^ byte[6] ^ byte[4] ^ byte[3] ^ byte[1]; //C1
        C[1] = msgBits[3] = byte[7] ^ byte[5] ^ byte[4] ^ byte[2] ^ byte[1]; //C2
        C[2] = msgBits[5] = byte[6] ^ byte[5] ^ byte[4] ^ byte[0]; //C4
        C[3] = msgBits[9] = byte[3] ^ byte[2] ^ byte[1] ^ byte[0]; //C8

        G = msgBits[1] = msgBits[2] ^ msgBits[3] ^ msgBits[4] ^ msgBits[5] ^ msgBits[6] ^ msgBits[7] ^ msgBits[8] ^ msgBits[9] ^ msgBits[10] ^ msgBits[11] ^ msgBits[12] ^ msgBits[13];
        printf("\n\ncodigo hamming:\n");
        for (int j = 13; j > 0; j--) {
            printf("%d ", msgBits[j]);
        }
        printf("\n");
        temp = fgetc(f);
        word[x] = temp;

        byte = decimalToBinary((int) temp);

        msgBits[13] = byte[0]; //M8
        msgBits[12] = byte[1]; //M7
        msgBits[11] = byte[2]; //M6
        msgBits[10] = byte[3]; //M5
        msgBits[8] = byte[4]; //M4
        msgBits[7] = byte[5]; //M3
        msgBits[6] = byte[6]; //M2
        msgBits[4] = byte[7]; //M1
        novoC[0] = msgBits[2] = msgBits[4] ^ msgBits[6] ^ msgBits[8] ^ msgBits[10] ^ msgBits[12]; //C1
        novoC[1] = msgBits[3] = msgBits[4] ^ msgBits[7] ^ msgBits[8] ^ msgBits[11] ^ msgBits[12]; //C2
        novoC[2] = msgBits[5] = msgBits[6] ^ msgBits[7] ^ msgBits[8] ^ msgBits[13]; //C4
        novoC[3] = msgBits[9] = msgBits[10] ^ msgBits[11] ^ msgBits[12] ^ msgBits[13]; //C8
        printf("\nverificando modificacoes...\n");
        for (int j = 13; j > 0; j--) {
            printf("%d ", msgBits[j]);
        }

        sindrome[0] = C[0] ^ novoC[0];
        sindrome[1] = C[1] ^ novoC[1];
        sindrome[2] = C[2] ^ novoC[2];
        sindrome[3] = C[3] ^ novoC[3];

        if (sindrome[0] == 0 && sindrome[1] == 0 && sindrome[2] == 0 && sindrome[3] == 0)
            printf("\n\nSEM ERRO!\n");
        else {
            pos = (sindrome[0] * 1 + sindrome[1] * 2 + sindrome[2] * 4 + sindrome[3] * 8) + 1;
            printf("\n\nerro na posicao %d!\n\ncorrigindo...\n\n", pos);
            if (msgBits[pos] == 1)
                msgBits[pos] = 0;
            else
                msgBits[pos] = 1;

            msgBits[2] = msgBits[4] ^ msgBits[6] ^ msgBits[8] ^ msgBits[10] ^ msgBits[12]; //C1
            msgBits[3] = msgBits[4] ^ msgBits[7] ^ msgBits[8] ^ msgBits[11] ^ msgBits[12]; //C2
            msgBits[5] = msgBits[6] ^ msgBits[7] ^ msgBits[8] ^ msgBits[13]; //C4
            msgBits[9] = msgBits[10] ^ msgBits[11] ^ msgBits[12] ^ msgBits[13];
            for (int j = 13; j > 0; j--) {
                printf("%d ", msgBits[j]);
            }
            novoG = msgBits[1] = msgBits[2] ^ msgBits[3] ^ msgBits[4] ^ msgBits[5] ^ msgBits[6] ^ msgBits[7] ^ msgBits[8] ^ msgBits[9] ^ msgBits[10] ^ msgBits[11] ^ msgBits[12] ^ msgBits[13];
            printf("\n\ncomparando Gs:\nG=%d novo G = %d\n", G, novoG);
            if (G != novoG) {
                printf("\npalavra rejeitada\n");
            } else {
                printf("\npalavra aceita\n");
                valor = msgBits[4] * 122 + msgBits[6] * 64 + msgBits[7] * 32 + msgBits[8] * 16 + msgBits[10] * 8 + msgBits[11] * 4 + msgBits[12] * 2 + msgBits[13] * 1;
                word[x] = valor;
            }

        }
        printf("\n---------------------------------------------\n");
        //system("read -p 'Aperte Enter para continuar...' var");
    }
    fclose(f);
    f = fopen(argv[1], "w");
    // printf("\npalavra final: ");
    for (int m = 0; m < size; m++) {
        //printf("%c",word[m]);
        fprintf(f, "%c", word[m]);
    }

    fclose(f);
    free(byte);
    free(msgBits);
    free(word);
}
