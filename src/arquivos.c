#include "dados.h"
#include "busca.h"

#include <stdio.h>

//ususario vai inserir produto
//funcao no main chama cria_produto, que retorna um produto
//funcao no main chama insercao_arquivo_produtos

#define QTD_PROD_ARQ 206876
#define QTD_EVEN_ARQ 109950743

//************CRIA REGIRSTRO************

void completa_com_espacos(char *str, int tamanho) {
    int i = strlen(str);
    while (i < tamanho-1) {
        str[i] = ' ';
        i++;
    }
    str[tamanho-1] = '\0';
}

PRODUTO cria_produto(long id_produto, char categoria[50], char marca[21], double preco){
    PRODUTO produto;
    produto.id_produto = id_produto;
    completa_com_espacos(categoria, 50);
    strcpy(produto.categoria, categoria);
    completa_com_espacos(marca, 21);
    strcpy(produto.marca, marca);
    produto.preco = preco;
    produto.excluido = false;
    produto.prox = 0;
    return produto;
}


//************INSERE NO ARQUIVO************

int insercao_arquivo_produtos(FILE *f, FILE *f_ind, PRODUTO novo){

    _fseeki64(f, 0, SEEK_SET);
    PRODUTO primeiro;
    if(fread(&primeiro, sizeof(PRODUTO), 1, f) != 1)
        return 0;   //nao leu

    if(novo.id_produto == primeiro.id_produto)
        return 0;   //já existe

    if(novo.id_produto < primeiro.id_produto){  //insere no inicio
        //antigo primeiro vai para o fim
        _fseeki64(f, 0, SEEK_END);
        long long posicao = _ftelli64(f);
        fwrite(&primeiro, sizeof(PRODUTO), 1, f);
        fputc('\n', f);
        

        //novo fica na primeira posicao do arquivo
        novo.prox = posicao;
        _fseeki64(f, 0, SEEK_SET);
        fwrite(&novo, sizeof(PRODUTO), 1, f);
        fputc('\n', f);

        //ajusta primeira entrada do indice
        INDICE_PROD ind;
        _fseeki64(f_ind, 0, SEEK_SET);
        if(fread(&ind, sizeof(INDICE_PROD), 1, f_ind) == 1){
            ind.chave = novo.id_produto;
            _fseeki64(f_ind, 0, SEEK_SET);
            fwrite(&ind, sizeof(INDICE_PROD), 1, f_ind); 
        }
        return 1;
    }
    else{ // insere no meio ou fim
        _fseeki64(f_ind, 0, SEEK_END);
        long long fim = _ftelli64(f_ind) / TAM_REG_IND_PROD;
        fim--; //endereco inicial do inserido
        long long endereco = pesquisa_binaria_indice(fim, TAM_REG_IND_PROD, f_ind, 0, novo.id_produto);
        if(endereco == -1)
            return 0;
        //endereco = endereco inicial do bloco
        

        //acha o endereco do registro que apontará para o adicionado (pos_ant)
        long long restantes = TAM_BLOCO;
        long long ate_fim = QTD_PROD_ARQ - endereco / TAM_REG_PROD;   //registros ordenados que faltam
        if(restantes > ate_fim)
            restantes = ate_fim;

        _fseeki64(f, endereco, SEEK_SET);
        int i;
        long long pos = endereco, pos_ant = endereco;
        PRODUTO lido;
        for(i=0;i<restantes;i++){
            if(fread(&lido, sizeof(PRODUTO), 1, f) != 1)
                return 0;   //nao conseguiu ler
            fgetc(f);       //consome \n
            if(lido.id_produto == novo.id_produto)
                return 0;   //produto ja existe
            if(lido.id_produto > novo.id_produto)
                break;      //anterior terá ele como proximo
            pos_ant = pos;
            pos += TAM_REG_PROD;
        }

        //segue os prox enquanto chave for menor que a do novo
        _fseeki64(f, pos_ant, SEEK_SET);
        PRODUTO anterior;
        if(fread(&anterior, sizeof(PRODUTO), 1, f) != 1)
            return 0;   //nao conseguiu ler
        while(anterior.prox != 0){
            _fseeki64(f, anterior.prox, SEEK_SET);
            if(fread(&lido, sizeof(PRODUTO), 1, f) != 1)
                return 0;
            if(lido.id_produto == novo.id_produto)
                return 0;   //ja existe
            if(lido.id_produto > novo.id_produto)
                break;
            pos_ant = anterior.prox;
            anterior = lido;
        }

        //grava novo no fim (herdando prox do seu anterior)
        novo.prox = anterior.prox;
        _fseeki64(f, 0, SEEK_END);
        long long pos_novo = _ftelli64(f);
        fwrite(&novo, sizeof(PRODUTO), 1, f);
        fputc('\n', f);

        //anterior passa a apontar para ele
        anterior.prox = pos_novo;
        _fseeki64(f, pos_ant, SEEK_SET);
        fwrite(&anterior, sizeof(PRODUTO), 1, f);

        return 1;
    
    }
}




void insercao_arquivo_eventos(FILE *f){
	_fseeki64(f, 0, SEEK_END);
    
}