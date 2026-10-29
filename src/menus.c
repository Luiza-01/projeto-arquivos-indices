void pesquisa_indice_p (FILE *f, FILE *f_ind){
    
    long long indice= 0;
    printf("Informe o índice a ser pesquisado:");
    scanf("%lld",&indice);
    busca_produto(f, f_ind, indice);
    
}

void pesquisa_indice_e (FILE *f, FILE *f_ind){
    
    long long indice= 0;
    printf("Informe o índice a ser pesquisado:");
    scanf("%lld",&indice);
    busca_evento(f, f_ind, indice);
    
}

void exclusao_p (FILE *f, FILE *f_ind){
    
    long long indice= 0;
    printf("Informe o índice do produto a ser excluido:");
    scanf("%lld",&indice);
    exclui_produto(f, f_ind, indice);
    
}

void exclusao_e (FILE *f, FILE *f_ind){
    
    long long indice= 0;
    printf("Informe o índice do evento a ser excluido:");
    scanf("%lld",&indice);
    exclui_evento(f, f_ind, indice);
    
}

void menu_produtos(){
    int escolha;
    
    do {
        printf("\n Arquivo de Produtos \n");
        printf("1) Pesquisa por índice \n");
        printf("2) Excluir produto \n");
        printf("3) Adicionar Produto \n");
        printf("0) Voltar \n");
        scanf("%d", &escolha);
        
        switch (escolha){
            
            case 1:
            
                printf("\n++ Pesquisa por Índice\n");
                    pesquisa_indice_p(f, f_ind);
                break;
                
            case 2:
            
                printf("\n++ Excluir Produto ++\n");
                    exclusao_p(f, f_ind);
                break;
                
            case 3:
            
                printf("\n++ Adicionar Produto ++\n");
                //add produto
                break;
            
            case 0:
            
                printf("\n Voltando ao menu principal...\n");
                break;
                
            default:
            
                printf("\n! Entrada Inválida ! \n");
                break;
        }
    } while (escolha !=0);
    
    
}


void menu_eventos(){
    int escolha;
    
    do {
        printf("\n Arquivo de Eventos \n");
        printf("1) Pesquisa por índice \n"); 
        printf("2) Excluir Evento \n");
        printf("3) Adicionar Evento \n");
        printf("0) Voltar \n");
        scanf("%d", &escolha);
        
        switch (escolha){
            
            case 1:
            
                printf("\n++ Pesquisa por Índice\n");
                    pesquisa_indice_e(f, f_ind);
                break;
                
            case 2:
            
                printf("\n++ Excluir Evento ++\n");
                exclusao_e(f, f_ind);
                break;
                
            case 3:
            
                printf("\n++ Adicionar Evento ++\n");
                //add evento
                break;
            
            case 0:
            
                printf("\n Voltando ao menu principal...\n");
                break;
                
            default:
            
                printf("\n! Entrada Inválida ! \n");
                break;
        }
    } while (escolha !=0);
    
    
}
