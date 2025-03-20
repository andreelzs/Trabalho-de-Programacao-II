#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>

struct cadastroCliente {
    char nome[50];
    char sexo;
    int idade;
};
typedef struct cadastroCliente cadastroCliente;

struct dadosVenda {
    cadastroCliente cliente;
    int numeroItens;
    char horarioCompra[6];
    float valorTotal;
};
typedef struct dadosVenda Venda;

void salvarNoArquivo(Venda* vendas, int totalVendas);
void carregarDoArquivo(Venda** vendas, int* totalVendas);
void cadastrarVendas(Venda** vendas, int* totalVendas);
void buscarVendaPorCliente(Venda* vendas, int totalVendas);
void exibirTodasVendas(Venda* vendas, int totalVendas);
void exibirInformacoesEspecificas(Venda* vendas, int totalVendas);
int carregarVendas(Venda** vendas);

int main() {
    Venda* vendas = NULL;
    int totalVendas = carregarVendas(&vendas);
    int loopMenu = 1;
    int opcaoMenu;

    while (loopMenu) {
        printf("\n----- Sistema Loja de Materiais -----\n");
        printf("Digite 1 para cadastrar novas vendas.\nDigite 2 para ver informações de uma venda específica, baseado no nome do cliente.\nDigite 3 para ver informações de todas as vendas.\nDigite 4 para encerrar o programa.\nDigite 5 para exibir informações específicas\n");
        scanf("%d", &opcaoMenu);

        switch (opcaoMenu) {
            case 1:
                cadastrarVendas(&vendas, &totalVendas);
                break;
            case 2:
                buscarVendaPorCliente(vendas, totalVendas);
                break;
            case 3:
                exibirTodasVendas(vendas, totalVendas);
                break;
            case 4:
                printf("Encerrando o programa...\n");
                loopMenu = 0;
                break;
            case 5:
            exibirInformacoesEspecificas(vendas, totalVendas);
            break;
            
            default:
                printf("Opção inválida. Tente novamente.\n");
        }
    }

    free(vendas);
    return 0;
}

void salvarNoArquivo(Venda* vendas, int totalVendas) {
    FILE *arquivo = fopen("arquivo.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    for (int i = 0; i < totalVendas; i++) {
        fprintf(arquivo, "%s %c %d %d %s %.2f\n",
                vendas[i].cliente.nome,
                vendas[i].cliente.sexo,
                vendas[i].cliente.idade,
                vendas[i].numeroItens,
                vendas[i].horarioCompra,
                vendas[i].valorTotal);
    }

    fclose(arquivo);
}

void carregarDoArquivo(Venda** vendas, int* totalVendas) {
    FILE *arquivo = fopen("arquivo.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return;
    }

    *totalVendas = 0;
    while (fscanf(arquivo, "%49s %c %d %d %5s %f\n",
                  (*vendas)[*totalVendas].cliente.nome,
                  &(*vendas)[*totalVendas].cliente.sexo,
                  &(*vendas)[*totalVendas].cliente.idade,
                  &(*vendas)[*totalVendas].numeroItens,
                  (*vendas)[*totalVendas].horarioCompra,
                  &(*vendas)[*totalVendas].valorTotal) == 6) {
        (*totalVendas)++;
        *vendas = realloc(*vendas, (*totalVendas + 1) * sizeof(Venda));
        if (*vendas == NULL) {
            printf("Erro ao alocar memória para vendas.\n");
            fclose(arquivo);
            return;
        }
    }

    fclose(arquivo);
}

void cadastrarVendas(Venda** vendas, int* totalVendas) {
    int quantidade;
    char continuar;
    while (1) {
        printf("Quantas vendas deseja cadastrar? ");
        scanf("%d", &quantidade);

        *vendas = realloc(*vendas, (*totalVendas + quantidade) * sizeof(Venda));
        if (*vendas == NULL) {
            printf("Erro ao alocar memória.\n");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < quantidade; i++) {
            Venda venda;

            printf("Nome do cliente: ");
            scanf(" %[^\n]s", venda.cliente.nome);
            if (strlen(venda.cliente.nome) < 4) {
                printf("Erro: Nome deve ter pelo menos 4 letras.\n");
                i--;
                continue;
            }

            printf("Sexo do cliente (m/f/n): ");
            scanf(" %c", &venda.cliente.sexo);
            if (venda.cliente.sexo != 'm' && venda.cliente.sexo != 'f' && venda.cliente.sexo != 'n') {
                printf("Erro: Sexo inválido.\n");
                i--;
                continue;
            }

            printf("Idade do cliente: ");
            scanf("%d", &venda.cliente.idade);
            if (venda.cliente.idade < 0) {
                printf("Erro: Idade não pode ser negativa.\n");
                i--;
                continue;
            }

            printf("Número de itens: ");
            scanf("%d", &venda.numeroItens);
            if (venda.numeroItens < 0) {
                printf("Erro: Número de itens não pode ser negativo.\n");
                i--;
                continue;
            }

            printf("Horário da compra (HH:MM): ");
            scanf("%s", venda.horarioCompra);
            int hora, minuto;
            if (sscanf(venda.horarioCompra, "%d:%d", &hora, &minuto) != 2 || hora < 0 || hora > 23 || minuto < 0 || minuto > 59) {
                printf("Erro: Horário inválido. Use o formato HH:MM.\n");
                i--;
                continue;
            }

            printf("Valor total: ");
            scanf("%f", &venda.valorTotal);
            if (venda.valorTotal < 0) {
                printf("Erro: Valor total não pode ser negativo.\n");
                i--;
                continue;
            }

            (*vendas)[*totalVendas] = venda;
            (*totalVendas)++;

            salvarNoArquivo(*vendas, *totalVendas);

            printf("Venda cadastrada com sucesso.\n");
        }

        printf("Deseja cadastrar mais vendas? (s/n) ");
        scanf(" %c", &continuar);
        if (continuar != 's' && continuar != 'S') {
            break; 
        }
    }
}

void buscarVendaPorCliente(Venda* vendas, int totalVendas) {
    char nomeCliente[50];
    int vendasCliente = 0;
    float somaValores = 0.0, maiorCompra = 0.0;
    char continuar;

    //laço do-while para perguntar se quer ir pro menu ou pesquisar novamente
    do{
        printf("Digite o nome do cliente para buscar as vendas: ");
        scanf(" %[^\n]", nomeCliente);
    
        // Encontra as venda pertencentes ao cliente digitado
        for (int i = 0; i < totalVendas; i++) {
            if (strcmp(vendas[i].cliente.nome, nomeCliente) == 0) {
                printf("\nVenda #%d\n", i + 1);
                printf("Valor Total: %.2f\n", vendas[i].valorTotal);
                printf("Hora da Compra: %s\n", vendas[i].horarioCompra);
                printf("Quantidade de Itens: %d\n", vendas[i].numeroItens);
    
                // Atualiza o total de vendas do cliente e soma dos valores
                vendasCliente++;
                somaValores += vendas[i].valorTotal;
    
                // Verifica o valor da compra mais cara
                if (vendas[i].valorTotal > maiorCompra) {
                    maiorCompra = vendas[i].valorTotal;
                }
            }
        }
    
        // Se o cliente tiver feito compras, exibe o resultado
        if (vendasCliente > 0) {
            float mediaCompras = somaValores / vendasCliente;
            printf("\nTotal de compras feitas por %s: %d\n", nomeCliente, vendasCliente);
            printf("Média do valor das compras: %.2f\n", mediaCompras);
            printf("Valor da compra mais cara: %.2f\n", maiorCompra);
        } else {
            printf("\nNão há compras registradas para o cliente %s.\n", nomeCliente);
        }
        printf("\nDeseja pesquisar novamente? (s/n): ");
        scanf(" %c", &continuar);
    }
    while (continuar == 's' || continuar == 'S');
    
}

void exibirTodasVendas(Venda* vendas, int totalVendas) {
    if (totalVendas == 0) {
        printf("Não há vendas cadastradas.\n");
        return;
    }

    printf("\nLista de todas as vendas:\n");
    for (int i = 0; i < totalVendas; i++) {
        printf("\nVenda %d:\n", i + 1);
        printf("Nome: %s\n", vendas[i].cliente.nome);
        printf("Sexo: %c\n", vendas[i].cliente.sexo);
        printf("Idade: %d\n", vendas[i].cliente.idade);
        printf("Número de itens: %d\n", vendas[i].numeroItens);
        printf("Horário da compra: %s\n", vendas[i].horarioCompra);
        printf("Valor total: %.2f\n", vendas[i].valorTotal);
    }
}

int carregarVendas(Venda** vendas) {
    FILE *arquivo = fopen("arquivo.txt", "r");
    if (arquivo == NULL) return 0;

    int totalVendas = 0;
    *vendas = malloc(sizeof(Venda));
    if (*vendas == NULL) {
        fclose(arquivo);
        return 0;
    }

    while (fscanf(arquivo, "%49s %c %d %d %5s %f\n",
                  (*vendas)[totalVendas].cliente.nome,
                  &(*vendas)[totalVendas].cliente.sexo,
                  &(*vendas)[totalVendas].cliente.idade,
                  &(*vendas)[totalVendas].numeroItens,
                  (*vendas)[totalVendas].horarioCompra,
                  &(*vendas)[totalVendas].valorTotal) == 6) {
        totalVendas++;
        *vendas = realloc(*vendas, (totalVendas + 1) * sizeof(Venda));
        if (*vendas == NULL) {
            printf("Erro ao alocar memória para vendas.\n");
            fclose(arquivo);
            return totalVendas;
        }
    }

    fclose(arquivo);
    return totalVendas;
}

void exibirInformacoesEspecificas(Venda* vendas, int totalVendas) {
    if (totalVendas == 0) {
        printf("Não há vendas cadastradas.\n");
        return;
    }

    // Variáveis para armazenar os totais e máximos
    int quantidadeVendas1Item = 0, vendasAntesDas12 = 0, compradoresHomens = 0;
    float totalComprasMulheres = 0.0, totalItensVendidos = 0.0, totalCompras = 0.0;
    float valorCompraMaisBarata = FLT_MAX, mediaCompras;
    char nomeMaior[50] = "", nomeMaisVelho[50] = "";
    int idadeMaisVelho = -1;

    for (int i = 0; i < totalVendas; i++) {
        if (vendas[i].numeroItens == 1) {
            quantidadeVendas1Item++;
        }

        int hora, minuto;
        sscanf(vendas[i].horarioCompra, "%d:%d", &hora, &minuto);
        if (hora < 12) {
            vendasAntesDas12++;
        }

        if (vendas[i].cliente.sexo == 'm') {
            compradoresHomens++;
        }
        
        if (vendas[i].cliente.sexo == 'f') {
            totalComprasMulheres += vendas[i].valorTotal;
        }
        totalItensVendidos += vendas[i].numeroItens;

        totalCompras += vendas[i].valorTotal;

        mediaCompras = totalCompras / (i + 1);

        if (vendas[i].valorTotal < valorCompraMaisBarata) {
            valorCompraMaisBarata = vendas[i].valorTotal;
        }

        if (strlen(vendas[i].cliente.nome) > strlen(nomeMaior)) {
            strcpy(nomeMaior, vendas[i].cliente.nome);
        }

        if (vendas[i].cliente.idade > idadeMaisVelho) {
            idadeMaisVelho = vendas[i].cliente.idade;
            strcpy(nomeMaisVelho, vendas[i].cliente.nome);
        }
    }

    // Exibindo os resultados
    printf("\nQuantidade de vendas com 1 item: %d\n", quantidadeVendas1Item);
    printf("Quantidade de vendas feitas antes das 12h: %d\n", vendasAntesDas12);
    printf("Quantidade de compradores homens: %d\n", compradoresHomens);
    printf("Valor total comprado por mulheres: %.2f\n", totalComprasMulheres);
    printf("Quantidade total de itens vendidos: %.0f\n", totalItensVendidos);
    printf("Valor total das compras: %.2f\n", totalCompras);
    printf("Valor médio de uma compra: %.2f\n", mediaCompras);
    printf("Valor da compra mais barata: %.2f\n", valorCompraMaisBarata);
    printf("Cliente com o maior nome: %s\n", nomeMaior);
    printf("Cliente mais velho: %s (Idade: %d)\n", nomeMaisVelho, idadeMaisVelho);

    // Exibindo as vendas dos clientes com o maior nome e mais velho
    for (int i = 0; i < totalVendas; i++) {
        if (strcmp(vendas[i].cliente.nome, nomeMaior) == 0) {
            printf("\nVenda do cliente com o maior nome (%s):\n", vendas[i].cliente.nome);
            printf("Nome: %s\n", vendas[i].cliente.nome);
            printf("Sexo: %c\n", vendas[i].cliente.sexo);
            printf("Idade: %d\n", vendas[i].cliente.idade);
            printf("Número de itens: %d\n", vendas[i].numeroItens);
            printf("Horário da compra: %s\n", vendas[i].horarioCompra);
            printf("Valor total: %.2f\n", vendas[i].valorTotal);
        }
        if (strcmp(vendas[i].cliente.nome, nomeMaisVelho) == 0) {
            printf("\nVenda do cliente mais velho (%s):\n", vendas[i].cliente.nome);
            printf("Nome: %s\n", vendas[i].cliente.nome);
            printf("Sexo: %c\n", vendas[i].cliente.sexo);
            printf("Idade: %d\n", vendas[i].cliente.idade);
            printf("Número de itens: %d\n", vendas[i].numeroItens);
            printf("Horário da compra: %s\n", vendas[i].horarioCompra);
            printf("Valor total: %.2f\n", vendas[i].valorTotal);
        }
    }

    char continuar;
    printf("\nDeseja retornar ao menu principal? (s/n): ");
    scanf(" %c", &continuar);

    if (continuar == 'n' || continuar == 'N') {
        printf("encerrando o programa...");
        exit(0);
    } else if (continuar == 's' || continuar == 'S') {
        return;
    } else {
        printf("Opção inválida! Retornando ao menu principal.\n");
        return;
    }
} 
