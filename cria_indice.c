//cada linha do arquivo produtos.bin tem 105 bytes (‘\n’ incluso)
//cada linha do arquivo eventos.bin tem 113 bytes (‘\n’ incluso)

//chave produto long id_produto
//chave evento long long id_evento

#include <stdio.h>
#include <stdbool.h>

//// ************CONSTANTES************

#define TAM_BLOCO 4096 //tamanho do bloco do índice
#define TAM_REG_PROD 105
#define TAM_REG_EVENTO 113

// ************STRUCTS************

typedef struct produto {
    long int id_produto;
    double preco;
    char marca[21];
    char categoria[50];
    long long prox;             //no arquivo, sem prox = 0 (dá para mudar pra -1, aí tem que mudar nos arquivos)
    bool excluido;
} PRODUTO;

typedef struct evento {
    long long id_evento;
    char id_sessao[37];
    long int id_user;
    char tipo_sessao[17];
    char horario[24];
    long long prox;
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
////************PESQUISA BINARIA************

void pesquisa_binaria(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado){
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
            return;
        if(chave == procurado){
            printf("Valor encontrado");
            return;
        }
        else if(chave < procurado)
            inicio = meio + 1;
        else 
            fim = meio - 1;
    }
    printf("Valor nao encontrado!\n");
}

//************BUSCA INDICES************
void busca_produto(){

}

// ************MAIN************

int main(){
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
