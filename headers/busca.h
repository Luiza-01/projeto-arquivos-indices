#ifndef BUSCA_H
#define BUSCA_H

long long pesquisa_binaria(long long fim, int tam_registro, FILE *f, int tipo_arquivo, long long procurado);

void escreve_produto(PRODUTO produto);

void escreve_evento(EVENTO evento);

#endif