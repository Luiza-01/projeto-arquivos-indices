#include "indices.h"
#include "arquivos.h"
#include "dados.h"

#include<stdio.h>

// ************MAIN************

int main(){
    printf("%d", sizeof(INDICE_EVENTO));
    printf("%d", sizeof(INDICE_PROD));
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

    //  MENUS
    
    int escolha;
    
    do {
        printf("E-commerce \n");
        printf("1) Arquivo de Produtos \n"); //Mexer Arquivo de Produtos
        printf("2) Arquivo de Eventos \n"); //Mexer Arquivo de Eventos
        printf("0) Encerrar programa \n");
        scanf("%d", &escolha);
        
        switch (escolha){
            
            case 1:
            
                printf("\n++ Arquivo de Produtos\n");
                menu_produtos();
                break;
                
            case 2:
            
                printf("\n++ Arquivo de Eventos\n");
                menu_eventos();
                break;
                
            case 0:
            
                printf("\n encerrando programa...\n");
                break;
                
            default:
            
                printf("\n! Entrada Inválida ! \n");
                break;
        }
    } while (escolha !=0);
    
    fclose(f_prod);
    fclose(f_evento);

    return 0;
}
