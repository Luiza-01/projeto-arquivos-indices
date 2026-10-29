#ifndef BUSCA_H
#define BUSCA_H

#include <stdio.h>
#include "dados.h"

long long pesquisa_binaria_arquivo(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado);

long long pesquisa_binaria_indice(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado);

void escreve_produto(PRODUTO produto);

void escreve_evento(EVENTO evento);

#endif