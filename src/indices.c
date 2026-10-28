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
void busca_produto(FILE *f, long long procurado){
    _fseeki64(f, 0, SEEK_END);
    long long fim = ftell(f) / TAM_REG_PROD;    //ftell(f) - tamanho em bytes, tamanho de um registro de produto
    fim --;                                     //qtd de produtos - 1 = ultima posicao
    long long posicao = busca_binaria(fim, sizeof(PRODUTO), f, 0, procurado);
    if(posicao == -1){
            printf("Entrada nao esta no arquivo ou e invalida");
            return;
    }
    _fseeki64(f, posicao * TAM_REG_PROD, SEEK_SET);
    PRODUTO produto;
    fread(&produto, sizeof(PRODUTO), 1, f);
    escreve_produto(produto);
}

void busca_evento(FILE *f, long long procurado){
    _fseeki64(f, 0, SEEK_END);
    long long fim = ftell(f) / TAM_REG_EVENTO;  //ftell(f) - tamanho em bytes, tamanho de um registro de evento
    fim --;                                     //qtd de eventos - 1 = ultima posicao
    long long posicao = busca_binaria(fim, sizeof(EVENTO), f, 0, procurado);
     if(posicao == -1){
            printf("Entrada nao esta no arquivo ou e invalida");
            return;
    }
    _fseeki64(f, posicao * TAM_REG_EVENTO, SEEK_SET);
    EVENTO evento;
    fread(&evento, sizeof(EVENTO), 1, f);
    escreve_evento(evento);
}

