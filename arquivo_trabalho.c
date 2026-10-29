#define __USE_MINGW_ANSI_STDIO 1 //para %lld do printf e scanf

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>

#define TAM_BLOCO 4096 			//tamanho do bloco do índice
#define TAM_REG_PROD 105		//tamanho de um registro de produto
#define TAM_REG_EVENTO 113		//tamanho de um registro de evento
#define TAM_REG_IND_PROD 8		//tamanho de um registro do indice de produto
#define TAM_REG_IND_EVENTO 16	//tamanho de um registro do indice de evento
#define QTD_PROD_ARQ 206876		//quantidade de registros de produto no arquivo
#define QTD_EVEN_ARQ 109950743	//quantidade de registros de produto no arquivo

// ************STRUCTS************

typedef struct produto {
    long int id_produto;
    double preco;
    char marca[21];
    char categoria[50];
    long long prox;             //no arquivo, sem prox = 0
    bool excluido;
} PRODUTO;

typedef struct evento {
    long long id_evento;
    char id_sessao[37];
    long int id_user;
    char tipo_sessao[17];
    char horario[24];
    long long prox;				//no arquivo, sem prox = 0
    bool excluido;
} EVENTO;

typedef struct indice_prod{
    long chave;
    long endereco; //endereco do primeiro registro do bloco
}INDICE_PROD;

typedef struct indice_evento{
    long long chave;
    long long endereco; //endereco do primeiro registro do bloco
}INDICE_EVENTO;

//************PESQUISAS BINARIAS************

//retorna numero do registro
long long pesquisa_binaria_arquivo(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado){
    long long inicio = 0, meio, chave, prox;
	long long endereco_anterior = -1, prox_anterior = 0;
    bool excluido;
    while(inicio <= fim){
		meio = inicio + (fim - inicio) / 2;  // para não ocorrer overflow, essa forma e mais segura 
		long long endereco = meio * tam_registro;
		
		if(_fseeki64(f, endereco, SEEK_SET) != 0){ //fseek que aceita 64 bits (devido ao long long)
			return -1;
		} 
        
		if(tipo_arquivo == 0){       //produto
            PRODUTO prod;
            if(fread(&prod, sizeof(PRODUTO), 1, f) != 1) 
				return -1;
            chave = prod.id_produto;
            prox = prod.prox;
            excluido = prod.excluido;
        }else if(tipo_arquivo == 1){ //evento
                EVENTO evento;
                if(fread(&evento, sizeof(EVENTO), 1, f) != 1) 
					return -1;
                chave = evento.id_evento;
                prox = evento.prox;
                excluido = evento.excluido;
        }else
            return -1;
        if(chave == procurado){
        	if(excluido)                        
                return -1;
            return endereco;
        }
        else if(chave < procurado){
        	endereco_anterior = endereco;
        	prox_anterior = prox;
            inicio = meio + 1;   
        }
        else 
            fim = meio - 1;
    }
    
    //verifica se nao esta na parte nao ordenada
    
    if(endereco_anterior == -1)
    	return -1;
    	
    prox = prox_anterior;
    while(prox != 0){
    	long long endereco_atual = prox;
    	
    	if(_fseeki64(f, endereco_atual, SEEK_SET) != 0)
    		return -1;
    	if(tipo_arquivo == 0){
            PRODUTO produto;
            if(fread(&produto, sizeof(PRODUTO), 1, f) != 1)
				return -1;
            chave = produto.id_produto;
            prox = produto.prox;
            excluido = produto.excluido;
        }else{
            EVENTO evento;
            if(fread(&evento, sizeof(EVENTO), 1, f) != 1)
				return -1;
            chave = evento.id_evento;
            prox = evento.prox;
            excluido = evento.excluido;
        }
        if(chave == procurado){
        	if(excluido)
        		return -1;
            return endereco_atual;
        }
        if(chave > procurado)
            return -1;  
    }
    return -1;
}

long long pesquisa_binaria_indice(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado){
    long long inicio = 0, meio, deslocamento_meio;
    long long endereco_bloco = -1;
    
    while(inicio <= fim){
        meio = inicio + (fim - inicio) / 2;  // para não ocorrer overflow, essa forma e mais segura 
		deslocamento_meio = meio * tam_registro;
		_fseeki64(f, deslocamento_meio, SEEK_SET); //fseek que aceita 64 bits (devido ao long long)
        long long chave, endereco;
        
        if(tipo_arquivo == 0){ //indice produto
            INDICE_PROD ind_prod;
            fread(&ind_prod, sizeof(INDICE_PROD), 1, f);
            chave = ind_prod.chave;
            endereco = ind_prod.endereco;
        }else if(tipo_arquivo == 1){ //indice evento
                INDICE_EVENTO ind_evento;
                fread(&ind_evento, sizeof(INDICE_EVENTO), 1, f);
                chave = ind_evento.chave;
                endereco = ind_evento.endereco;
        }else
            return -1;
            
        if(chave <= procurado){
            endereco_bloco = endereco;
            inicio = meio + 1;
        }
        else 
            fim = meio - 1;
    }
    return endereco_bloco;
}

//************ ESCRITA ************

void escreve_produto(PRODUTO prod){
    printf("\n");
    printf("ID: %ld ", prod.id_produto);
    printf("PRECO: %f ", prod.preco);
    printf("MARCA: %s ", prod.marca);
    printf("CATEGORIA: %s", prod.categoria);
    printf("\n");
}
void escreve_evento(EVENTO evento){
    printf("\n");
    printf("%lld ", evento.id_evento);
    printf("%s ", evento.horario);
    printf("%ld ", evento.id_user);
    printf("%s ", evento.id_sessao);
    printf("%s", evento.tipo_sessao);
    printf("\n");
}

//************CRIACAO DE INDICES************

void cria_indice_prod(FILE *f){
    FILE *f_ind = fopen("indice_produto.bin", "wb");
    if(f_ind == NULL){
        printf("Arquivo de indice nao criado!\n");
        return;
    }
    _fseeki64(f, 0, SEEK_SET);
    PRODUTO produto;
    long posicao_atual = 0;
    while(posicao_atual < QTD_PROD_ARQ && fread(&produto, sizeof(produto), 1, f) == 1){
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
    FILE *f_ind = fopen("indice_evento.bin", "wb");
    if(f_ind == NULL){
        printf("Arquivo de indice nao criado!\n");
        return;
    }
    _fseeki64(f, 0, SEEK_SET);
    EVENTO evento;
    long long posicao_atual = 0;
    while(posicao_atual < QTD_EVEN_ARQ && fread(&evento, sizeof(evento), 1, f) == 1){
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
    
    long long restantes = TAM_BLOCO;
    long long ate_fim = QTD_PROD_ARQ - endereco / TAM_REG_PROD;
    if(restantes > ate_fim)
        restantes = ate_fim;
        
    PRODUTO produto, anterior;
    memset(&anterior, 0, sizeof(PRODUTO));      //anterior.prox = 0 caso nada seja lido
    bool achou = false;
    int i;    
    
    _fseeki64(f, endereco, SEEK_SET);

    for(i=0;i<restantes;i++){
    	if(fread(&produto, sizeof(PRODUTO), 1, f) != 1)
    		break;	//fim arquivo
    	fgetc(f);	//consome \n
    	if(produto.id_produto == procurado){
    		achou = true;
			break;	
    	}
    	if(produto.id_produto > procurado)
    		break; //numero e maior que o procurado, procurado nao esta no arquivo
    	anterior = produto;
	}
    while(!achou && anterior.prox != 0){
        _fseeki64(f, anterior.prox, SEEK_SET);
        if(fread(&produto, sizeof(PRODUTO), 1, f) != 1)
            break;
        if(produto.id_produto == procurado){
            achou = true;
            break;
        }
        if(produto.id_produto > procurado)
            break;                              //cadeia e crescente: nao existe
        anterior = produto;
    }

    if(achou && !produto.excluido)
        escreve_produto(produto);
    else
        printf("Entrada nao esta no arquivo ou e invalida");
}

void busca_evento(FILE *f, FILE *f_ind, long long procurado){
    _fseeki64(f_ind, 0, SEEK_END);                          //corrigido: tamanho do INDICE, nao do arquivo de dados
    long long fim = _ftelli64(f_ind) / TAM_REG_IND_EVENTO;
    fim --;
    long long endereco = pesquisa_binaria_indice(fim, TAM_REG_IND_EVENTO, f_ind, 1, procurado);
    if(endereco == -1){
        printf("Entrada nao esta no arquivo ou e invalida");
        return;
    }

    long long restantes = TAM_BLOCO;
    long long ate_fim = QTD_EVEN_ARQ - endereco / TAM_REG_EVENTO;
    if(restantes > ate_fim)
        restantes = ate_fim;

    EVENTO evento, anterior;
    memset(&anterior, 0, sizeof(EVENTO));
    bool achou = false;
    int i;

    _fseeki64(f, endereco, SEEK_SET);
    
    for(i=0;i<restantes;i++){
        if(fread(&evento, sizeof(EVENTO), 1, f) != 1)
            break;
        fgetc(f);
        if(evento.id_evento == procurado){
            achou = true;
            break;
        }
        if(evento.id_evento > procurado)
            break;
        anterior = evento;
    }

    while(!achou && anterior.prox != 0){
        _fseeki64(f, anterior.prox, SEEK_SET);
        if(fread(&evento, sizeof(EVENTO), 1, f) != 1)
            break;
        if(evento.id_evento == procurado){
            achou = true;
            break;
        }
        if(evento.id_evento > procurado)
            break;
        anterior = evento;
    }

    if(achou && !evento.excluido)
        escreve_evento(evento);
    else
        printf("Entrada nao esta no arquivo ou e invalida");
}

//************INSERCAO************

void completa_com_espacos(char *dest, const char *src, int tamanho) {
    int n = strcspn(src, "\r\n");

    if (n > tamanho - 1)
        n = tamanho - 1;

    memcpy(dest, src, n);
    memset(dest + n, ' ', tamanho - 1 - n);
    dest[tamanho - 1] = '\0';
}

PRODUTO cria_produto(long id_produto, char categoria[50], char marca[21], double preco){
    PRODUTO produto;
    memset(&produto, 0, sizeof(PRODUTO));
    produto.id_produto = id_produto;
    completa_com_espacos(produto.categoria, categoria, 50);
    completa_com_espacos(produto.marca, marca, 21);
    produto.preco = preco;
    produto.excluido = false;
    produto.prox = 0;
    return produto;
}

EVENTO cria_evento(long long id_evento, char id_sessao[37], long id_user, char tipo_sessao[17], char horario[24]){
    EVENTO evento;
    memset(&evento, 0, sizeof(EVENTO));
    evento.id_evento = id_evento;
    completa_com_espacos(evento.id_sessao, id_sessao, 37);
    evento.id_user = id_user;
    completa_com_espacos(evento.tipo_sessao, tipo_sessao, 17);
    completa_com_espacos(evento.horario, horario, 24);
    evento.excluido = false;
    evento.prox = 0;
    return evento;
}

int insercao_arquivo_produtos(FILE *f, FILE *f_ind, PRODUTO novo){
    _fseeki64(f, 0, SEEK_SET);
    PRODUTO primeiro;
    if(fread(&primeiro, sizeof(PRODUTO), 1, f) != 1)
        return 0;   //nao leu

    if(novo.id_produto == primeiro.id_produto){
    	if(!primeiro.excluido)
        	return 0;   //já existe
        novo.prox = primeiro.prox;
        _fseeki64(f, 0, SEEK_SET);
        fwrite(&novo, sizeof(PRODUTO), 1, f);
        return 1;
	}
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
            if(lido.id_produto == novo.id_produto){
             	if(!lido.excluido)
                    return 0;   //produto ja existe
                novo.prox = lido.prox;                  //reativa no mesmo lugar
                _fseeki64(f, pos, SEEK_SET);
                fwrite(&novo, sizeof(PRODUTO), 1, f);
                return 1;
            }
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
            if(lido.id_produto == novo.id_produto){
                if(!lido.excluido)
                    return 0;   //ja existe
                novo.prox = lido.prox;                  //reativa no mesmo lugar
                _fseeki64(f, anterior.prox, SEEK_SET);
                fwrite(&novo, sizeof(PRODUTO), 1, f);
                return 1;
            }
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

int insercao_arquivo_eventos(FILE *f, FILE *f_ind, EVENTO novo){
    _fseeki64(f, 0, SEEK_SET);
    EVENTO primeiro;
    if(fread(&primeiro, sizeof(EVENTO), 1, f) != 1)
        return 0;   //nao leu

    if(novo.id_evento == primeiro.id_evento){
    	if(!primeiro.excluido)
        	return 0;   //já existe
        novo.prox = primeiro.prox;
        _fseeki64(f, 0, SEEK_SET);
        fwrite(&novo, sizeof(EVENTO), 1, f);
        return 1;
	}
    if(novo.id_evento < primeiro.id_evento){  //insere no inicio
        //antigo primeiro vai para o fim
        _fseeki64(f, 0, SEEK_END);
        long long posicao = _ftelli64(f);
        fwrite(&primeiro, sizeof(EVENTO), 1, f);
        fputc('\n', f);
        
        //novo fica na primeira posicao do arquivo
        novo.prox = posicao;
        _fseeki64(f, 0, SEEK_SET);
        fwrite(&novo, sizeof(EVENTO), 1, f);
        fputc('\n', f);

        //ajusta primeira entrada do indice
        INDICE_EVENTO ind;
        _fseeki64(f_ind, 0, SEEK_SET);
        if(fread(&ind, sizeof(INDICE_EVENTO), 1, f_ind) == 1){
            ind.chave = novo.id_evento;
            _fseeki64(f_ind, 0, SEEK_SET);
            fwrite(&ind, sizeof(INDICE_EVENTO), 1, f_ind); 
        }
        return 1;
    }
    else{ // insere no meio ou fim
        _fseeki64(f_ind, 0, SEEK_END);
        long long fim = _ftelli64(f_ind) / TAM_REG_IND_EVENTO;
        fim--; //endereco inicial do inserido
        long long endereco = pesquisa_binaria_indice(fim, TAM_REG_IND_EVENTO, f_ind, 1, novo.id_evento);
        if(endereco == -1)
            return 0;
        //endereco = endereco inicial do bloco
        

        //acha o endereco do registro que apontará para o adicionado (pos_ant)
        long long restantes = TAM_BLOCO;
        long long ate_fim = QTD_EVEN_ARQ - endereco / TAM_REG_EVENTO;   //registros ordenados que faltam
        if(restantes > ate_fim)
            restantes = ate_fim;

        _fseeki64(f, endereco, SEEK_SET);
        int i;
        long long pos = endereco, pos_ant = endereco;
        EVENTO lido;
        for(i=0;i<restantes;i++){
            if(fread(&lido, sizeof(EVENTO), 1, f) != 1)
                return 0;   //nao conseguiu ler
            fgetc(f);       //consome \n
            if(lido.id_evento == novo.id_evento){
             	if(!lido.excluido)
                    return 0;   //produto ja existe
                novo.prox = lido.prox;                  //reativa no mesmo lugar
                _fseeki64(f, pos, SEEK_SET);
                fwrite(&novo, sizeof(EVENTO), 1, f);
                return 1;
            }
            if(lido.id_evento > novo.id_evento)
                break;      //anterior terá ele como proximo
            pos_ant = pos;
            pos += TAM_REG_EVENTO;
        }

        //segue os prox enquanto chave for menor que a do novo
        _fseeki64(f, pos_ant, SEEK_SET);
        EVENTO anterior;
        if(fread(&anterior, sizeof(EVENTO), 1, f) != 1)
            return 0;   //nao conseguiu ler
        while(anterior.prox != 0){
            _fseeki64(f, anterior.prox, SEEK_SET);
            if(fread(&lido, sizeof(EVENTO), 1, f) != 1)
                return 0;
            if(lido.id_evento == novo.id_evento){
                if(!lido.excluido)
                    return 0;   //ja existe
                novo.prox = lido.prox;                  //reativa no mesmo lugar
                _fseeki64(f, anterior.prox, SEEK_SET);
                fwrite(&novo, sizeof(EVENTO), 1, f);
                return 1;
            }
            if(lido.id_evento > novo.id_evento)
                break;
            pos_ant = anterior.prox;
            anterior = lido;
        }

        //grava novo no fim (herdando prox do seu anterior)
        novo.prox = anterior.prox;
        _fseeki64(f, 0, SEEK_END);
        long long pos_novo = _ftelli64(f);
        fwrite(&novo, sizeof(EVENTO), 1, f);
        fputc('\n', f);

        //anterior passa a apontar para ele
        anterior.prox = pos_novo;
        _fseeki64(f, pos_ant, SEEK_SET);
        fwrite(&anterior, sizeof(EVENTO), 1, f);
        return 1;
    }
}

//************REMOCAO************

int remocao_arquivo_produtos(FILE *f, FILE *f_ind, long long procurado){
	//achar bloco com pesquisa binaria no indice
	_fseeki64(f_ind, 0, SEEK_END);
    long long fim = _ftelli64(f_ind) / TAM_REG_IND_PROD;
    fim--;
    long long endereco = pesquisa_binaria_indice(fim, TAM_REG_IND_PROD, f_ind, 0, procurado);
    //'endereco' tem o endereco inicial do bloco
	
	if(endereco == -1)
        return 0;       //menor que a primeira chave: nao existe
   
    //limita quantidade de registros lidos (o ultimo bloco pode ser menor que TAM_BLOCO)
    long long restantes = TAM_BLOCO;
    long long ate_fim = QTD_PROD_ARQ - (endereco / TAM_REG_PROD); // qtd de produtos do arquivo - posicao do primeiro registro do bloco = qtd de registros na parte ordenada do bloco
    if(restantes > ate_fim)
        restantes = ate_fim;

    PRODUTO produto, anterior;
    memset(&anterior, 0, sizeof(PRODUTO));
    long long posicao = endereco, pos_achou = -1;
    int i;

    //ler parte ordenada
    _fseeki64(f, endereco, SEEK_SET);
    for(i=0;i<restantes;i++){
        if(fread(&produto, sizeof(PRODUTO), 1, f) != 1)
            break;		//nao conseguiu ler
        fgetc(f);       //consome \n
        if(produto.id_produto == procurado){ //achou
            pos_achou = posicao; 
            break;
        }
        if(produto.id_produto > procurado) //chave não encontrada
            break;
        //menor - avanca
        anterior = produto;
        posicao += TAM_REG_PROD;
    }

    //parte fora da ordenacao (prox)
    while(pos_achou == -1 && anterior.prox != 0){
        posicao = anterior.prox;
        _fseeki64(f, posicao, SEEK_SET);
        if(fread(&produto, sizeof(PRODUTO), 1, f) != 1)
            break;
        if(produto.id_produto == procurado){
            pos_achou = posicao;
            break;
        }
        if(produto.id_produto > procurado)
            break;
        anterior = produto;
    }

    if(pos_achou == -1 || produto.excluido)
        return 0;

    //regrava o registro com excluido = true 
    produto.excluido = true;
    _fseeki64(f, pos_achou, SEEK_SET);
    fwrite(&produto, sizeof(PRODUTO), 1, f);
    fflush(f);
    return 1;
}

int remocao_arquivo_eventos(FILE *f, FILE *f_ind, long long procurado){
	//achar bloco com pesquisa binaria no indice
	_fseeki64(f_ind, 0, SEEK_END);
    long long fim = _ftelli64(f_ind) / TAM_REG_IND_EVENTO;
    fim--;
    long long endereco = pesquisa_binaria_indice(fim, TAM_REG_IND_EVENTO, f_ind, 1, procurado);
    //'endereco' tem o endereco inicial do bloco
	
	if(endereco == -1)
        return 0;       //menor que a primeira chave: nao existe
   
    //limita quantidade de registros lidos (o ultimo bloco pode ser menor que TAM_BLOCO)
    long long restantes = TAM_BLOCO;
    long long ate_fim = QTD_EVEN_ARQ - (endereco / TAM_REG_EVENTO); // qtd de produtos do arquivo - posicao do primeiro registro do bloco = qtd de registros na parte ordenada do bloco
    if(restantes > ate_fim)
        restantes = ate_fim;

    EVENTO evento, anterior;
    memset(&anterior, 0, sizeof(EVENTO));
    long long posicao = endereco, pos_achou = -1;
    int i;

    //ler parte ordenada
    _fseeki64(f, endereco, SEEK_SET);
    for(i=0;i<restantes;i++){
        if(fread(&evento, sizeof(EVENTO), 1, f) != 1)
            break;		//nao conseguiu ler
        fgetc(f);       //consome \n
        if(evento.id_evento == procurado){ //achou
            pos_achou = posicao; 
            break;
        }
        if(evento.id_evento > procurado) //chave não encontrada
            break;
        //menor - avanca
        anterior = evento;
        posicao += TAM_REG_EVENTO;
    }

    //parte fora da ordenacao (prox)
    while(pos_achou == -1 && anterior.prox != 0){
        posicao = anterior.prox;
        _fseeki64(f, posicao, SEEK_SET);
        if(fread(&evento, sizeof(EVENTO), 1, f) != 1)
            break;
        if(evento.id_evento == procurado){
            pos_achou = posicao;
            break;
        }
        if(evento.id_evento > procurado)
            break;
        anterior = evento;
    }

    if(pos_achou == -1 || evento.excluido)
        return 0;

    //regrava o registro com excluido = true 
    evento.excluido = true;
    _fseeki64(f, pos_achou, SEEK_SET);
    fwrite(&evento, sizeof(EVENTO), 1, f);
    fflush(f);
    return 1;
}

//************ PESQUISAS ************

void pesquisa_sequencial_arquivo_produtos(FILE *f, long long qtd_desejada){
	if(f == NULL || qtd_desejada <= 0){
		printf("Busca inválida!\n");
		return;	
	}
	
	PRODUTO produto;
	long long posicao_arq, posicao_prox, cont = 0;
	long long i;
	_fseeki64(f, 0, SEEK_SET);
	for(i=0; i<QTD_PROD_ARQ && cont < qtd_desejada; i++){
		if(fread(&produto, sizeof(PRODUTO), 1, f) == 1){
			fgetc(f);
			if(!produto.excluido){
				escreve_produto(produto);
				cont++;	
			}
			if(produto.prox != 0){
				posicao_arq = _ftelli64(f); // posicao atual do arquivo
				posicao_prox = produto.prox;
				while(posicao_prox != 0 && cont < qtd_desejada){
					_fseeki64(f, posicao_prox, SEEK_SET);
					if (fread(&produto, sizeof(PRODUTO), 1, f) != 1)
           		 		 break;
           		 	if(!produto.excluido){
            			escreve_produto(produto);
            			cont++;	
            		}
					posicao_prox = produto.prox;
				}	
				_fseeki64(f, posicao_arq, SEEK_SET);
			}
		}
	}
}
void pesquisa_sequencial_arquivo_eventos(FILE *f, long long qtd_desejada){
	if(f == NULL || qtd_desejada <= 0){
		printf("Busca inválida!\n");
		return;	
	}
	
	EVENTO evento;
	long long posicao_arq, posicao_prox, cont = 0;
	long long i;
	_fseeki64(f, 0, SEEK_SET);
	for(i=0; i < QTD_EVEN_ARQ && cont < qtd_desejada; i++){
		if(fread(&evento, sizeof(EVENTO), 1, f) == 1){
			fgetc(f);
			if(!evento.excluido){
				escreve_evento(evento);	
				cont++;
			}
			if(evento.prox != 0){
				posicao_arq = _ftelli64(f); // posicao atual do arquivo
				posicao_prox = evento.prox;
				while(posicao_prox != 0 && cont < qtd_desejada){
					_fseeki64(f, posicao_prox, SEEK_SET);
					if (fread(&evento, sizeof(EVENTO), 1, f) != 1)
           		 		 break;
           		 	if(!evento.excluido){
            			escreve_evento(evento);
            			cont++;	
            		}
					posicao_prox = evento.prox;
				}	
				_fseeki64(f, posicao_arq, SEEK_SET);
			}
		}
	}
}

void pesquisa_binaria_arquivo_produtos(FILE *f, long procurado){
	long long endereco = pesquisa_binaria_arquivo(QTD_PROD_ARQ - 1, TAM_REG_PROD, f, 0, procurado);
	if(endereco == -1){
		printf("Produto nao encontrado!\n");
		return;	
	}
	PRODUTO produto;

    _fseeki64(f, endereco, SEEK_SET);

    if (fread(&produto, sizeof(PRODUTO), 1, f) == 1) {
        if(!produto.excluido)
            escreve_produto(produto);
        else
            printf("Produto excluido!\n");
    }else 
        printf("Erro ao ler produto!\n");
    
}

void pesquisa_binaria_arquivo_eventos(FILE *f, long long procurado){	
	long long endereco = pesquisa_binaria_arquivo(QTD_EVEN_ARQ - 1, TAM_REG_EVENTO, f, 1, procurado);
    if (endereco == -1) {
        printf("Evento nao encontrado!\n");
        return;
    }
    EVENTO evento;

    _fseeki64(f, endereco, SEEK_SET);

    if(fread(&evento, sizeof(EVENTO), 1, f) == 1) {
        if(!evento.excluido)
            escreve_evento(evento);
        else
            printf("Evento excluido!\n");
    }else
        printf("Erro ao ler evento!\n");
}

long long quantidade_registros_produto(FILE *f) {
    if (_fseeki64(f, 0, SEEK_END) != 0)
        return -1;
    long long tamanho = _ftelli64(f);
    if (tamanho < 0 || tamanho % TAM_REG_PROD != 0)
        return -1;
        
    return tamanho / TAM_REG_PROD;
}

void produto_mais_caro(FILE *f){
	long long quantidade = quantidade_registros_produto(f);

    if (quantidade < 0) {
        printf("Erro ao ler o arquivo de produtos.\n");
        return;
    }	
    
    PRODUTO mais_caro;
    bool encontrado = false;
	long long i;
    for (i = 0; i < quantidade; i++){
        long long endereco = i * TAM_REG_PROD;
        if (_fseeki64(f, endereco, SEEK_SET) != 0) {
            printf("Erro ao acessar o arquivo.\n");
            return;
        }
        PRODUTO produto;
        if (fread(&produto, sizeof(PRODUTO), 1, f) != 1) {
            printf("Erro ao ler um produto.\n");
            return;
        }

        if (!produto.excluido && (!encontrado || produto.preco > mais_caro.preco)) {
            mais_caro = produto;
            encontrado = true;
        }
    }

    if (!encontrado) {
        printf("Nenhum produto encontrado.\n");
        return;
    }

    printf("\nProduto com maior preco:\n");
    escreve_produto(mais_caro);
}

void consulta_produtos_categoria(FILE *f){
    if (f == NULL) {
        printf("Arquivo de produtos invalido!\n");
        return;
    }

    char categoria[50];
    char categoria_busca[50];
	while (getchar() != '\n'); //limpa buffer
    printf("Digite a categoria: ");
    fgets(categoria, sizeof(categoria), stdin);
    categoria[strcspn(categoria, "\r\n")] = '\0';
    completa_com_espacos(categoria_busca, categoria, 50);

    PRODUTO produto;
    long long i;
    int encontrados = 0;

    _fseeki64(f, 0, SEEK_SET);

    for (i = 0; i < QTD_PROD_ARQ; i++) {
        if (fread(&produto, sizeof(PRODUTO), 1, f) != 1) 
            break;
        fgetc(f); // Consome o '\n' do registro

        if (!produto.excluido && strcmp(produto.categoria, categoria_busca) == 0){
            escreve_produto(produto);
            encontrados++;
        }
    }
    if (encontrados == 0)
        printf("Nenhum produto ativo encontrado nessa categoria.\n");
    else
        printf("\nTotal de produtos encontrados: %d\n", encontrados);
}

//************ RECONSTRUCAO DE INDICE ************

//nao usada pq o indice nao muda! (Somente sobre parte fixa)
void reconstruir_indice(FILE *f, FILE **f_ind, char *f_nome, int tipo){
    fflush(f);
    fclose(*f_ind);
    if(tipo == 0)
    	cria_indice_prod(f);
    else if(tipo == 1)
    	cria_indice_evento(f);
    *f_ind = fopen(f_nome, "r+b");
    if(*f_ind == NULL)
        printf("Nao foi possivel reabrir o arquivo de indice!\n");
    else
        printf("Indice reconstruido (somente parte ordenada).\n");
}

//************ MENUS ************

void menu_produtos(FILE *f, FILE *f_ind){
    int escolha;
    do {
        printf("\nArquivo de Produtos\n");
        printf("1) Pesquisa por indice\n");
        printf("2) Pesquisa binaria direto no arquivo\n");
        printf("3) Mostrar produtos\n");
        printf("4) Excluir produto\n");
        printf("5) Adicionar Produto\n");
        printf("6) Mostrar produto mais caro\n");
        printf("7) Mostrar produtos de uma categoria\n");
        printf("0) Voltar\n");
        scanf("%d", &escolha);
        
        switch(escolha){
            case 1: {
            	long procurado;
                printf("\nPesquisa por Indice\n");
                printf("ID procurado: \n");
                scanf("%ld", &procurado);
                busca_produto(f, f_ind, procurado);
                break;
            }
            case 2: {
            	long procurado;
                printf("\nPesquisa Binaria direto no arquivo\n");
                printf("ID procurado: \n");
                scanf("%ld", &procurado);
                pesquisa_binaria_arquivo_produtos(f, procurado);    
                break;
            }
            case 3: {
            	long qtd;
                printf("\nMostrar produtos\n");
                printf("Mostrar quantos?");
                scanf("%ld", &qtd);
                pesquisa_sequencial_arquivo_produtos(f, qtd);
                break;
            }
            case 4: {
            	long long procurado;
                printf("\nExcluir produto\n");
                printf("Id do registro a excluir: ");
                scanf("%lld", &procurado);
                remocao_arquivo_produtos(f, f_ind, procurado);
                break;
            }
            case 5: {
            	long id_produto;
            	char categoria[50], marca[21];
            	double preco;
            	
                printf("\nAdicionar Produto\n");
                printf("Id produto: ");
                scanf("%ld", &id_produto);
                while (getchar() != '\n'); //limpa buffer
                printf("Categoria: ");
                fgets(categoria, sizeof(categoria), stdin);
                categoria[strcspn(categoria, "\r\n")] = '\0';
                printf("Marca: ");
                fgets(marca, sizeof(marca), stdin);
                marca[strcspn(marca, "\r\n")] = '\0';
                printf("Preco: ");
                scanf("%lf", &preco);
            	
				PRODUTO novo = cria_produto(id_produto, categoria, marca, preco);
                insercao_arquivo_produtos(f, f_ind, novo);
                break;
            }
            case 6: {
            	printf("\nMostrar mais caro:\n");	
            	produto_mais_caro(f);
            	break;
            }
            case 7: {
            	printf("\nMostrar produtos de uma categoria:\n");	
            	consulta_produtos_categoria(f);
            	break;
            }
            case 0:     
                printf("\nVoltando ao menu principal...\n");
                break; 
            default:
                printf("\n! Entrada Invalida !\n");
                break;
        }
    }while(escolha !=0); 
}

void menu_eventos(FILE *f, FILE *f_ind){
    int escolha;
    do {
        printf("\nArquivo de Eventos\n");
        printf("1) Pesquisa por indice\n");
        printf("2) Pesquisa binaria direto no arquivo\n");
        printf("3) Mostrar eventos\n");
        printf("4) Excluir evento\n");
        printf("5) Adicionar evento\n");
        printf("0) Voltar\n");
        scanf("%d", &escolha);
        
        switch(escolha){
            case 1: {
            	long long procurado;
                printf("\nPesquisa por Indice\n");
                printf("ID procurado:");
                scanf("%lld", &procurado);
                busca_evento(f, f_ind, procurado);
                break;
            }
            case 2: {
            	long long procurado;
                printf("\nPesquisa Binaria direto no arquivo\n");
                printf("ID procurado:");
                scanf("%lld", &procurado);
                pesquisa_binaria_arquivo_eventos(f, procurado);    
                break;
            }
            case 3: {
                long long qtd;
                printf("\nMostrar eventos\n");
                printf("Mostrar quantos?");
                scanf("%lld", &qtd);
                pesquisa_sequencial_arquivo_eventos(f, qtd);
                break;
            }
            case 4:{
            	long long procurado;
                printf("\nExcluir evento\n");
                printf("Id do registro a excluir: ");
                scanf("%lld", &procurado);
                remocao_arquivo_eventos(f, f_ind, procurado);
                break;
            }
            case 5: {
            	long long id_evento;
            	long id_user;
            	char id_sessao[37], tipo_sessao[17], horario[24];
            	
                printf("\nAdicionar evento\n");
                printf("ID evento: ");
                scanf("%lld", &id_evento);
                printf("ID user: ");
                scanf("%ld", &id_user);
                while (getchar() != '\n'); //limpa buffer
                printf("ID sessao: ");
                fgets(id_sessao, sizeof(id_sessao), stdin);
                id_sessao[strcspn(id_sessao, "\r\n")] = '\0';
                printf("Tipo sessao: ");
                fgets(tipo_sessao, sizeof(tipo_sessao), stdin);
                tipo_sessao[strcspn(tipo_sessao, "\r\n")] = '\0';
                printf("Horario: ");
                fgets(horario, sizeof(horario), stdin);
    			horario[strcspn(horario, "\r\n")] = '\0';
            	
				EVENTO novo = cria_evento(id_evento, id_sessao, id_user, tipo_sessao, horario);
                insercao_arquivo_eventos(f, f_ind, novo);
                break;
            }
            case 0:     
                printf("\nVoltando ao menu principal...\n");
                break; 
            default:
                printf("\n! Entrada Invalida !\n");
                break;
        }
    }while(escolha !=0); 
}


int main(){
    FILE *f_prod = fopen("produtos.bin", "r+b");
    if(f_prod == NULL){
        printf("Nao foi possivel abrir o arquivo!\n");
        return 1;
	}
    FILE *f_evento = fopen("eventos.bin", "r+b");
        if(f_evento == NULL){
        printf("Nao foi possivel abrir o arquivo!\n");
        return 1;
    }
     
    FILE *f_ind_prod = fopen("indice_produto.bin", "r+b");
    if(f_ind_prod == NULL){
        cria_indice_prod(f_prod);
        f_ind_prod = fopen("indice_produto.bin", "r+b");
    }
    FILE *f_ind_evento = fopen("indice_evento.bin", "r+b");
    if(f_ind_evento == NULL){
        cria_indice_evento(f_evento);       
        f_ind_evento = fopen("indice_evento.bin", "r+b");
    }
    if(f_ind_prod == NULL || f_ind_evento == NULL){
        printf("Nao foi possivel abrir o arquivo de indice!\n");
        return 1;
    }

	int escolha;
    
    do {
        printf("E-commerce \n");
        printf("1) Arquivo de Produtos \n");
        printf("2) Arquivo de Eventos \n"); 
        printf("0) Encerrar programa \n");
        scanf("%d", &escolha);
        
        switch (escolha){
            case 1:
                printf("\nArquivo de Produtos\n");
                menu_produtos(f_prod, f_ind_prod);
                break;
            case 2:
                printf("\nArquivo de Eventos\n");
                menu_eventos(f_evento, f_ind_evento);
                break;
            case 0:
                printf("\nencerrando programa...\n");
                break;     
            default:
                printf("\n! Entrada Inválida ! \n");
                break;
        }
    }while (escolha !=0);
	
    fclose(f_prod);
    fclose(f_evento);
    fclose(f_ind_prod);
    fclose(f_ind_evento);
    
    return 0;
}