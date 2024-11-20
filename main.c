#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <time.h> 
  
typedef enum { VERMELHO, LARANJA, AMARELO } Criticidade; 
typedef enum { ATENDIDO, EM_ANDAMENTO, ENCERRADO } StatusProcesso; 
typedef enum { ARMA_DE_FOGO, DROGA_ILICITA, CONTRABANDO, ARMA_BRANCA } Categoria; 
typedef enum { VERIFICACAO_MANUAL, INVESTIGACAO, BLOQUEIO_DE_AREA } AcoesRecomendadas; 
typedef enum { PORTAO_A, PORTAO_B, PORTAO_C, PORTAO_D } Localizacao; 
  
typedef struct { 
    int id; 
    Criticidade criticidade; 
    StatusProcesso status_processo; 
    Categoria categoria; 
    char *observacao; 
    AcoesRecomendadas acoes_recomendadas; 
    Localizacao localizacao; 
    struct tm horario; 
} AlertaEmergencia; 
  
AlertaEmergencia *criar_alerta(int id, Criticidade criticidade, StatusProcesso status, 
                               Categoria categoria, const char *obs, AcoesRecomendadas acoes, 
                               Localizacao local) { 
    AlertaEmergencia *alerta = (AlertaEmergencia *)malloc(sizeof(AlertaEmergencia)); 
    alerta->id = id; 
    alerta->criticidade = criticidade; 
    alerta->status_processo = status; 
    alerta->categoria = categoria; 
    alerta->observacao = strdup(obs); 
    alerta->acoes_recomendadas = acoes; 
    alerta->localizacao = local; 
    time_t t = time(NULL); 
    alerta->horario = *localtime(&t); 
    return alerta; 
} 
  
char *gerar_comando_sql_insert(const AlertaEmergencia *alerta) { 
    const char *criticidades[] = {"Vermelho", "Laranja", "Amarelo"}; 
    const char *status[] = {"Atendido", "Em andamento", "Encerrado"}; 
    const char *categorias[] = {"Arma de fogo", "Droga ilícita", "Contrabando", "Arma branca"}; 
    const char *acoes[] = {"Verificação manual", "Investigação", "Bloqueio de área"}; 
    const char *locais[] = {"Portão A", "Portão B", "Portão C", "Portão D"}; 
  
    char *sql = (char *)malloc(512); 
    if (!sql) { 
        perror("Erro ao alocar memória"); 
        exit(EXIT_FAILURE); 
    } 

    snprintf(sql, 512, 
             "INSERT INTO alertas_emergencias " 
             "(criticidade, status_processo, categoria, observacao, " 
             "acoes_recomendadas, localizacao, horario) " 
             "VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%04d-%02d-%02d %02d:%02d:%02d');", 
             criticidades[alerta->criticidade], status[alerta->status_processo], 
             categorias[alerta->categoria], alerta->observacao, 
             acoes[alerta->acoes_recomendadas], locais[alerta->localizacao], 
             alerta->horario.tm_year + 1900, alerta->horario.tm_mon + 1, alerta->horario.tm_mday, 
             alerta->horario.tm_hour, alerta->horario.tm_min, alerta->horario.tm_sec); 
    return sql; 
} 
  
char *gerar_comando_sql_delete(int id) { 
    char *sql = (char *)malloc(512); 
    if (!sql) { 
        perror("Erro ao alocar memória"); 
        exit(EXIT_FAILURE); 
    } 
  
    snprintf(sql, 512, 
             "DELETE FROM alertas_emergencias WHERE id = %d;", id); 
    return sql; 
} 
  
void mostrar_alerta_completo(const AlertaEmergencia *alerta) { 
    const char *criticidades[] = {"Vermelho", "Laranja", "Amarelo"}; 
    const char *status[] = {"Atendido", "Em andamento", "Encerrado"}; 
    const char *categorias[] = {"Arma de fogo", "Droga ilícita", "Contrabando", "Arma branca"}; 
    const char *acoes[] = {"Verificação manual", "Investigação", "Bloqueio de área"}; 
    const char *locais[] = {"Portão A", "Portão B", "Portão C", "Portão D"}; 
  
    printf("\nDetalhes do Alerta:\n"); 
    printf("ID: %d\n", alerta->id); 
    printf("Criticidade: %s\n", criticidades[alerta->criticidade]); 
    printf("Status: %s\n", status[alerta->status_processo]); 
    printf("Categoria: %s\n", categorias[alerta->categoria]); 
    printf("Localização: %s\n", locais[alerta->localizacao]); 
    printf("Observação: %s\n", alerta->observacao); 
    printf("Ação recomendada: %s\n", acoes[alerta->acoes_recomendadas]); 
    printf("Data e hora: %04d-%02d-%02d %02d:%02d:%02d\n", 
           alerta->horario.tm_year + 1900, alerta->horario.tm_mon + 1, alerta->horario.tm_mday, 
           alerta->horario.tm_hour, alerta->horario.tm_min, alerta->horario.tm_sec); 
} 
  
void liberar_alerta(AlertaEmergencia *alerta) { 
    if (alerta) { 
        free(alerta->observacao); 
        free(alerta); 
    } 
} 

int ler_opcao(const char *prompt, int max) { 
    int opcao; 
    do { 
        printf("%s", prompt); 
        scanf("%d", &opcao); 
    } while (opcao < 0 || opcao >= max); 
    return opcao; 
} 
  
int main() { 
    static int id_counter = 1; 
    AlertaEmergencia *alertas[100]; 
    int count = 0; 
    int continuar = 1; 
  
    while (continuar) { 
        printf("\nEscolha a operação (0: INSERT, 1: SELECT, 2: Mostrar Todos, 3: Atualizar, 4: Remover, 5: Sair): "); 
        int operacao = ler_opcao("Opção: ", 6); 
  
        if (operacao == 5) { 
            printf("Encerrando o programa...\n"); 
            break; 
        } 
  
        if (operacao == 0) { 
            printf("Selecione a Criticidade (0: Vermelho, 1: Laranja, 2: Amarelo): "); 
            Criticidade criticidade = ler_opcao("Opção: ", 3); 
            printf("Selecione o Status (0: Atendido, 1: Em andamento, 2: Encerrado): "); 
            StatusProcesso status = ler_opcao("Opção: ", 3); 
            printf("Selecione a Categoria (0: Arma de fogo, 1: Droga ilícita, 2: Contrabando, 3: Arma branca): "); 
            Categoria categoria = ler_opcao("Opção: ", 4); 
            printf("Digite a Observação: "); 
  
            char buffer[256]; 
            getchar(); 
            fgets(buffer, 256, stdin); 
            buffer[strcspn(buffer, "\n")] = '\0'; 
  
            printf("Selecione as Ações Recomendadas (0: Verificação manual, 1: Investigação, 2: Bloqueio de área): "); 
            AcoesRecomendadas acoes = ler_opcao("Opção: ", 3); 
            printf("Selecione a Localização (0: Portão A, 1: Portão B, 2: Portão C, 3: Portão D): "); 
            Localizacao local = ler_opcao("Opção: ", 4); 
  
            AlertaEmergencia *alerta = criar_alerta(id_counter++, criticidade, status, 
                                                     categoria, buffer, acoes, local); 
            alertas[count++] = alerta; 
  
            char *sql = gerar_comando_sql_insert(alerta); 
            printf("\nComando SQL Gerado:\n%s\n", sql); 
            free(sql); 
        } else if (operacao == 1) { 
            printf("Selecione o ID do Alerta para buscar: "); 
            int id_busca; 
            scanf("%d", &id_busca); 
  
            int encontrado = 0; 
            for (int i = 0; i < count; i++) { 
                if (alertas[i]->id == id_busca) { 
                    mostrar_alerta_completo(alertas[i]); 
                    encontrado = 1; 
                    break; 
                } 
            } 
            if (!encontrado) { 
                printf("Alerta não encontrado com ID %d.\n", id_busca); 
            } 
        } else if (operacao == 2) { 
            for (int i = 0; i < count; i++) { 
                mostrar_alerta_completo(alertas[i]); 
            } 
        } else if (operacao == 3) { 
            printf("Digite o ID do Alerta para atualizar: "); 
            int id_atualizar; 
            scanf("%d", &id_atualizar); 
  
            int encontrado = 0; 
            for (int i = 0; i < count; i++) { 
                if (alertas[i]->id == id_atualizar) { 
                    printf("Alerta encontrado! Selecione o campo a ser alterado:\n"); 
                    printf("1. Observação\n"); 
                    printf("2. Status\n"); 
                    printf("3. Criticidade\n"); 
  
                    int campo = ler_opcao("Opção: ", 4); 
  
                    char novo_valor[256]; 
                    printf("Digite o novo valor: "); 
                    getchar(); 
                    fgets(novo_valor, 256, stdin); 
                    novo_valor[strcspn(novo_valor, "\n")] = '\0'; 
  
                    if (campo == 1) { 
                        free(alertas[i]->observacao); 
                        alertas[i]->observacao = strdup(novo_valor); 
                    } else if (campo == 2) { 
                        alertas[i]->status_processo = (StatusProcesso)(novo_valor[0] - '0'); 
                    } else if (campo == 3) { 
                        alertas[i]->criticidade = (Criticidade)(novo_valor[0] - '0'); 
                    } 
  
                    char *sql = gerar_comando_sql_insert(alertas[i]); 
                    printf("\nComando SQL Gerado para UPDATE:\n%s\n", sql); 
                    free(sql); 
                    encontrado = 1; 
                    break; 
                } 
            } 
            if (!encontrado) { 
                printf("Alerta não encontrado com ID %d.\n", id_atualizar); 
            } 
        } else if (operacao == 4) { 
            printf("Digite o ID do Alerta para remover: "); 
            int id_remover; 
            scanf("%d", &id_remover); 
  
            int encontrado = 0; 
            for (int i = 0; i < count; i++) { 
                if (alertas[i]->id == id_remover) { 
                    char *sql = gerar_comando_sql_delete(id_remover); 
                    printf("\nComando SQL Gerado para DELETE:\n%s\n", sql); 
                    free(sql); 
  
                    liberar_alerta(alertas[i]); 
                    for (int j = i; j < count - 1; j++) { 
                        alertas[j] = alertas[j + 1]; 
                    } 
                    count--; 
                    encontrado = 1; 
                    break; 
                } 
            } 
            if (!encontrado) { 
                printf("Alerta não encontrado com ID %d.\n", id_remover); 
            } 
        } 
    } 
  
    for (int i = 0; i < count; i++) { 
        liberar_alerta(alertas[i]); 
    } 
  
    return 0; 
}
