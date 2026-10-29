#include "dados.h"
#include "busca.h"
#include <stdio.h>

void exclui_produto(FILE *f, FILE *f_ind, long long procurado){
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
    		produto.excluido = true;
        printf("Produto excluido")
			return;	
    	}
    	if(produto.id_produto >= procurado)
    		break; //numero e maior que o procurado, procurado nao esta no arquivo
    }
    printf("Entrada nao esta no arquivo ou e invalida"); 
    
}

void busca_evento(FILE *f, FILE *f_ind, long long procurado){
    _fseeki64(f, 0, SEEK_END);
    long long fim = ftell(f) / TAM_REG_IND_EVENTO;  //ftell(f) - tamanho em bytes, tamanho de um registro de evento
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
    		evento.excluido = true;
        printf("Evento excluido")
			return;	
    	}
    	if(evento.id_evento >= procurado)
    		break; //numero e maior que o procurado, procurado nao esta no arquivo
    }
    printf("Entrada nao esta no arquivo ou e invalida");      
}
