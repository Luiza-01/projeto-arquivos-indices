#include "dados.h"

#include <stdio.h>

busca_evento_arquivo(FILE *f){
    _fseeki64(f, 0, SEEK_END);
    long long fim = ftell(f) / TAM_REG_EVENTO;  //ftell(f) - tamanho em bytes, tamanho de um registro de evento
    fim --;
    
}