//cada linha do arquivo produtos.bin tem 105 bytes (‘\n’ incluso)
//cada linha do arquivo eventos.bin tem 113 bytes (‘\n’ incluso)

//chave produto long id_produto
//chave evento long long id_evento
#include "dados.h"
#include "busca.h"
#include <stdio.h>


// ************CRIACAO INDICES************

void cria_indice_prod(FILE *f){
    FILE *f_ind = fopen("indice_produto", "wb");
    if(f_ind == NULL){
        printf("Arquivo de indice nao criado!\n");
        return;
    }
    
    PRODUTO produto;
    long posicao_atual = 0;
    while(fread(&produto, sizeof(produto), 1, f) == 1){
        fgetc(f);   //consome com o \n, que está no arquivo, mas não na estrutura PRODUTO
        if(posicao_atual % TAM_BLOCO == 0){
            INDICE_PROD ind;
            ind.chave = produto.id_produto;
            ind.endereco = posicao_atual * TAM_REG_PROD;
            fwrite(&ind, sizeof(ind), 1, f_ind);
        }
        posicao_atual++;
    }

    fclose(f_ind);
}

void cria_indice_evento(FILE *f){
    FILE *f_ind = fopen("indice_evento", "wb");
    if(f_ind == NULL){
        printf("Arquivo de indice nao criado!\n");
        return;
    }
    
    EVENTO evento;
    long posicao_atual = 0;
    while(fread(&evento, sizeof(evento), 1, f) == 1){
        fgetc(f);   //consome com o \n, que está no arquivo, mas não na estrutura PRODUTO
        if(posicao_atual % TAM_BLOCO == 0){
            INDICE_EVENTO ind;
            ind.chave = evento.id_evento;
            ind.endereco = posicao_atual * TAM_REG_EVENTO;
            fwrite(&ind, sizeof(ind), 1, f_ind);
        }
        posicao_atual++;
    }

    fclose(f_ind);
}

//************BUSCA INDICES************
void busca_produto(){
    
    busca_binaria(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado);
}

void busca_evento(){

    busca_binaria();
}

