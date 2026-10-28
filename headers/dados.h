#ifndef DADOS_H
#define DADOS_H

#include<stdbool.h>

#define TAM_BLOCO 4096 //tamanho do bloco do índice
#define TAM_REG_PROD 105
#define TAM_REG_EVENTO 113
#define TAM_REG_IND_PROD 8
#define TAM_REG_IND_EVENTO 16

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

#endif