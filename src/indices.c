//cada linha do arquivo produtos.bin tem 105 bytes (‘\n’ incluso)
//cada linha do arquivo eventos.bin tem 113 bytes (‘\n’ incluso)

//chave produto long id_produto
//chave evento long long id_evento
#include "dados.h"
#include "busca.h"
#include <stdio.h>


// ************CRIACAO INDICES************

void cria_indice_prod(FILE *f){
    FILE *f_ind = fopen("indice_produto.bin", "wb");
    if(f_ind == NULL){
        printf("Arquivo de indice nao criado!\n");
        return;
    }
    FILE *ft = fopen("indice_produto.txt", "w");
    PRODUTO produto;
    long posicao_atual = 0;
    while(fread(&produto, sizeof(produto), 1, f) == 1){
        fgetc(f);   //consome com o \n, que está no arquivo, mas não na estrutura PRODUTO
        if(posicao_atual % TAM_BLOCO == 0){
            INDICE_PROD ind;
            ind.chave = produto.id_produto;
            ind.endereco = posicao_atual * TAM_REG_PROD;
            fwrite(&ind, sizeof(ind), 1, f_ind);
            fprintf(ft, "%ld %ld\n", ind.chave, ind.endereco);
        }
        posicao_atual++;
    }

    fclose(f_ind);
    fclose(ft);
}

void cria_indice_evento(FILE *f){
    FILE *f_ind = fopen("indice_evento.bin", "wb");
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

void busca_produto(FILE *f, FILE *f_ind, long long procurado){
    _fseeki64(f_ind, 0, SEEK_END);
    long long fim = ftell(f_ind) / TAM_REG_IND_PROD;    //ftell(f) - tamanho em bytes, tamanho de um registro de produto
    fim --;                                     //qtd de produtos - 1 = ultima posicao
	long long endereco = pesquisa_binaria_indice(fim, TAM_REG_IND_PROD, f_ind, 0, procurado);
    if(endereco == -1){
        printf("Entrada nao esta no arquivo ou e invalida");
        return;
    }
    _fseeki64(f, endereco, SEEK_SET);
    PRODUTO produto;
    int i;
    for(i=0;i<TAM_BLOCO;i++){
    	if(fread(&produto, sizeof(PRODUTO), 1, f) != 1)
    		break;	//fim arquivo
    	fgetc(f);	//consome \n
    	if(produto.id_produto == procurado && !produto.excluido){
    		escreve_produto(produto);
			return;	
    	}
    	if(produto.id_produto >= procurado)
    		break; //numero e maior que o procurado, procurado nao esta no arquivo
    }
    printf("Entrada nao esta no arquivo ou e invalida"); 
    
}

void busca_evento(FILE *f, FILE *f_ind, long long procurado){
    _fseeki64(f_ind, 0, SEEK_END);
    long long fim = ftell(f_ind) / TAM_REG_IND_EVENTO;  //ftell(f) - tamanho em bytes, tamanho de um registro de evento
    fim --;                                     //qtd de eventos - 1 = ultima posicao
    long long endereco = pesquisa_binaria(fim, TAM_REG_IND_EVENTO, f_ind, 1, procurado);
    if(endereco == -1){
        printf("Entrada nao esta no arquivo ou e invalida");
        return;
    }
    _fseeki64(f, endereco, SEEK_SET);
    EVENTO evento;
    int i;
    for(i=0;i<TAM_BLOCO;i++){
    	if(fread(&evento, sizeof(EVENTO), 1, f) != 1)
    		break;	//fim arquivo
    	fgetc(f);	//consome \n
    	if(evento.id_evento == procurado && !evento.excluido){
    		escreve_evento(evento);
			return;	
    	}
    	if(evento.id_evento >= procurado)
    		break; //numero e maior que o procurado, procurado nao esta no arquivo
    }
    printf("Entrada nao esta no arquivo ou e invalida");      
}

