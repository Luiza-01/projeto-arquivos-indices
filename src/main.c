#include "indices.h"
#include "arquivos.h"
#include "dados.h"

#include<stdio.h>

// ************MAIN************

int main(){
    printf("%d", sizeof(INDICE_EVENTO));
    printf("%d", sizeof(INDICE_PROD));
    FILE *f_prod = fopen("produtos.bin", "rb");
    if(f_prod == NULL){
        printf("Nao foi possivel abrir o arquivo!\n");
        return 1;
    }

    FILE *f_evento = fopen("eventos.bin", "rb");
        if(f_evento == NULL){
        printf("Nao foi possivel abrir o arquivo!\n");
        return 1;
    }
    cria_indice_prod(f_prod);

    fclose(f_prod);
    fclose(f_evento);

    return 0;
}