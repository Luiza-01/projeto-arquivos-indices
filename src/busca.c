#include "dados.h"
#include "busca.h"
#include <stdio.h>

////************PESQUISA BINARIA************

long long pesquisa_binaria(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado){
    long long inicio = 0, meio, deslocamento_meio;
    fim --;
    while(inicio <= fim){
        meio = inicio + (fim - inicio) / 2;  // para não ocorrer overflow, essa forma e mais segura 
        deslocamento_meio = meio * tam_registro;
        _fseeki64(f, deslocamento_meio, SEEK_SET); //fseek que aceita 64 bits (devido ao long long)
        long long chave;
        if(tipo_arquivo == 0){       //produto
            PRODUTO prod;
            fread(&prod, sizeof(PRODUTO), 1, f);
            chave = prod.id_produto;
        }else if(tipo_arquivo == 1){ //indice produto
                INDICE_PROD ind_prod;
                fread(&ind_prod, sizeof(INDICE_PROD), 1, f);
                chave = ind_prod.chave;
        }else if(tipo_arquivo == 2){ //evento
                EVENTO evento;
                fread(&evento, sizeof(EVENTO), 1, f);
                chave = evento.id_evento;
        }else if(tipo_arquivo == 3){ //indice evento
                INDICE_EVENTO ind_evento;
                fread(&ind_evento, sizeof(INDICE_EVENTO), 1, f);
                chave = ind_evento.chave;
        }else
            return -1;
        if(chave == procurado){
            printf("Valor encontrado");
            return meio;
        }
        else if(chave < procurado)
            inicio = meio + 1;
        else 
            fim = meio - 1;
    }
    printf("Valor nao encontrado!\n");
    return -1;
}

//por enquanto fica aqui
void escreve_produto(PRODUTO prod){
    printf("\n");
    printf("ID: %ld", prod.id_produto);
    printf("PRECO: %f", prod.preco);
    printf("MARCA: %s", prod.marca);
    printf("CATEGORIA: %s", prod.categoria);
    printf("\n");
}
void escreve_evento(EVENTO evento){
    printf("\n");
    printf("%ld", evento.id_evento);
    printf("%s", evento.horario);
    printf("%ld", evento.id_user);
    printf("%s", evento.id_sessao);
    printf("%s", evento.tipo_sessao);
    printf("\n");
}