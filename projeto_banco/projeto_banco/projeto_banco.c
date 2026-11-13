#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <time.h>

#define MAX_CONTAS 100
#define MAX_MOVIMENTOS 500

typedef struct {
    char nif[10];
    char nome[51];
    char senha[21];
    float saldo;
} Conta;

typedef struct {
    char nif[10];
    char tipo[15];
    float valor;
    char data[20];
} Movimento;

void menu(void);
int login(Conta contas[], int cont_conta, char nif_logado[]);
int procurarConta(Conta contas[], int cont_conta, char nif[]);
void ordenarPorSaldo(Conta contas[], int cont_conta);
void eliminarConta(Conta contas[], int* cont_conta, int pos);

int main()
{
    setlocale(LC_ALL, "pt_PT.UTF8");

    int opcao = 0, cont_conta = 0, cont_movimento = 0;
    int i, pos, logado = 0;
    char nif_logado[10];
    char senha_confirmar[21];
    float valor;
    time_t agora;
    struct tm* tm_info;
    char data_str[20];
    char nif_destino[10];
    int pos_destino, n, confirmar;

    Conta contas[MAX_CONTAS];
    Movimento movimentos[MAX_MOVIMENTOS];

    do {
        /* --------- MENU INICIAL (ainda sem login) --------- */
        if (!logado) {
            printf("\n---BANCO---");
            printf("\n1-Criar Conta");
            printf("\n2-Entrar (Login)");
            printf("\n-1-Sair");
            printf("\n\nIntroduza sua opcao: ");
            scanf("%d", &opcao);

            switch (opcao) {

            case 1:
                printf("\n---Criacao de conta---\n");

                if (cont_conta >= MAX_CONTAS) {
                    printf("\nNao e possivel criar mais contas (limite atingido).");
                    break;
                }

                printf("Introduza o NIF (9 digitos): ");
                scanf("%9s", contas[cont_conta].nif);

                /* verificar se o NIF ja existe */
                if (procurarConta(contas, cont_conta, contas[cont_conta].nif) != -1) {
                    printf("\nJa existe uma conta com esse NIF!!");
                    break;
                }

                printf("Introduza o nome: ");
                scanf(" %50[^\n]", contas[cont_conta].nome);

                printf("Introduza a senha: ");
                scanf("%20s", contas[cont_conta].senha);

                printf("Confirme a senha: ");
                scanf("%20s", senha_confirmar);

                if (strcmp(contas[cont_conta].senha, senha_confirmar) != 0) {
                    printf("\nAs senhas nao coincidem! Conta nao foi criada.");
                    break;
                }

                contas[cont_conta].saldo = 0;
                cont_conta++;

                printf("\nConta criada com sucesso!!");
                break;

            case 2:
                logado = login(contas, cont_conta, nif_logado);
                if (!logado) {
                    printf("\nNIF ou senha incorretos!!");
                }
                break;
            }
        }

        /* --------- MENU DA CONTA (depois de login) --------- */
        else {
            pos = procurarConta(contas, cont_conta, nif_logado);

            menu();
            printf("\nIntroduza sua opcao: ");
            scanf("%d", &opcao);

            switch (opcao) {

            case 1:
                printf("\n----SALDO: %.2f", contas[pos].saldo);
                break;

            case 2:
                printf("\n---Historico de movimentos---\n");
                if (cont_movimento == 0) {
                    printf("Nenhum movimento registado...");
                }
                else {
                    for (i = 0; i < cont_movimento; i++) {
                        if (strcmp(movimentos[i].nif, nif_logado) == 0) {
                            printf("\n%s | %s | %.2f", movimentos[i].data, movimentos[i].tipo, movimentos[i].valor);
                        }
                    }
                }
                break;

            case 3:
                printf("\nIntroduza o valor a depositar: ");
                scanf("%f", &valor);

                if (valor <= 0) {
                    printf("\nValor invalido!!");
                    break;
                }

                contas[pos].saldo += valor;

                if (cont_movimento < MAX_MOVIMENTOS) {
                    strcpy(movimentos[cont_movimento].nif, nif_logado);
                    strcpy(movimentos[cont_movimento].tipo, "Deposito");
                    movimentos[cont_movimento].valor = valor;

                    time(&agora);
                    tm_info = localtime(&agora);
                    strftime(data_str, sizeof(data_str), "%d/%m/%Y %H:%M", tm_info);
                    strcpy(movimentos[cont_movimento].data, data_str);

                    cont_movimento++;
                }

                printf("\nDeposito efetuado com sucesso!! Novo saldo: %.2f", contas[pos].saldo);
                break;

            case 4:
                printf("\nIntroduza o valor a levantar: ");
                scanf("%f", &valor);

                if (valor <= 0) {
                    printf("\nValor invalido!!");
                    break;
                }

                if (valor > contas[pos].saldo) {
                    printf("\nSaldo insuficiente!!");
                    break;
                }

                contas[pos].saldo -= valor;

                if (cont_movimento < MAX_MOVIMENTOS) {
                    strcpy(movimentos[cont_movimento].nif, nif_logado);
                    strcpy(movimentos[cont_movimento].tipo, "Levantamento");
                    movimentos[cont_movimento].valor = valor;

                    time(&agora);
                    tm_info = localtime(&agora);
                    strftime(data_str, sizeof(data_str), "%d/%m/%Y %H:%M", tm_info);
                    strcpy(movimentos[cont_movimento].data, data_str);

                    cont_movimento++;
                }

                printf("\nLevantamento efetuado com sucesso!! Novo saldo: %.2f", contas[pos].saldo);
                break;

            case 5:
                printf("\n---Transferencia---\n");
                printf("Introduza o NIF da conta destino: ");
                scanf("%9s", nif_destino);

                if (strcmp(nif_destino, nif_logado) == 0) {
                    printf("\nNao pode transferir para a propria conta!!");
                    break;
                }

                pos_destino = procurarConta(contas, cont_conta, nif_destino);

                if (pos_destino == -1) {
                    printf("\nConta destino nao encontrada!!");
                    break;
                }

                printf("Introduza o valor a transferir: ");
                scanf("%f", &valor);

                if (valor <= 0) {
                    printf("\nValor invalido!!");
                    break;
                }

                if (valor > contas[pos].saldo) {
                    printf("\nSaldo insuficiente!!");
                    break;
                }

                /* saldo nunca pode ficar negativo em nenhuma das contas */
                contas[pos].saldo -= valor;
                contas[pos_destino].saldo += valor;

                time(&agora);
                tm_info = localtime(&agora);
                strftime(data_str, sizeof(data_str), "%d/%m/%Y %H:%M", tm_info);

                if (cont_movimento < MAX_MOVIMENTOS) {
                    strcpy(movimentos[cont_movimento].nif, nif_logado);
                    strcpy(movimentos[cont_movimento].tipo, "Transf.Enviada");
                    movimentos[cont_movimento].valor = valor;
                    strcpy(movimentos[cont_movimento].data, data_str);
                    cont_movimento++;
                }

                if (cont_movimento < MAX_MOVIMENTOS) {
                    strcpy(movimentos[cont_movimento].nif, nif_destino);
                    strcpy(movimentos[cont_movimento].tipo, "Transf.Recebida");
                    movimentos[cont_movimento].valor = valor;
                    strcpy(movimentos[cont_movimento].data, data_str);
                    cont_movimento++;
                }

                printf("\nTransferencia efetuada com sucesso!! Novo saldo: %.2f", contas[pos].saldo);
                break;

            case 6:
                printf("\n---Top contas com maior saldo---\n");

                if (cont_conta == 0) {
                    printf("Nenhuma conta registada...");
                    break;
                }

                ordenarPorSaldo(contas, cont_conta);

                /* pos tinha sido calculado antes de ordenar; recalcula apos ordenar */
                pos = procurarConta(contas, cont_conta, nif_logado);

                printf("Quantas contas quer ver no top? ");
                scanf("%d", &n);

                if (n > cont_conta) {
                    n = cont_conta;
                }
                if (n < 1) {
                    n = 1;
                }

                for (i = 0; i < n; i++) {
                    printf("\n%d. %s - Saldo: %.2f", i + 1, contas[i].nome, contas[i].saldo);
                }
                break;

            case 7:
                printf("\n---Eliminar conta---\n");
                printf("Tem a certeza? Esta accao e irreversivel. (1-Sim / 0-Nao): ");
                scanf("%d", &confirmar);

                if (confirmar == 1) {
                    eliminarConta(contas, &cont_conta, pos);
                    logado = 0;
                    printf("\nConta eliminada com sucesso.");
                }
                else {
                    printf("\nOperacao cancelada.");
                }
                break;

            case 8:
                logado = 0;

                printf("\nSessao terminada.");
                break;

            case -1:
                opcao = -1;
                break;
            }
        }

    } while (opcao != -1);

    return 0;
}

void menu(void)
{
    printf("\n\n\n\n---BANCO (conta autenticada)---");
    printf("\n1-Ver saldo");
    printf("\n2-Historico de movimentos");
    printf("\n3-Depositar");
    printf("\n4-Levantar");
    printf("\n5-Transferir para outra conta");
    printf("\n6-Top contas com maior saldo");
    printf("\n7-Eliminar a minha conta");
    printf("\n8-Logout");
    printf("\n-1-Sair");
}

/* ordena as contas por saldo decrescente*/
void ordenarPorSaldo(Conta contas[], int cont_conta)
{
    int i, j;
    Conta temp;

    for (i = 0; i < cont_conta - 1; i++) {
        for (j = 0; j < cont_conta - 1 - i; j++) {
            if (contas[j].saldo < contas[j + 1].saldo) {
                temp = contas[j];
                contas[j] = contas[j + 1];
                contas[j + 1] = temp;
            }
        }
    }
}

/* elimina a conta na posicao 'pos', deslocando as seguintes uma posicao para tras */
void eliminarConta(Conta contas[], int* cont_conta, int pos)
{
    int i;

    for (i = pos; i < (*cont_conta) - 1; i++) {
        contas[i] = contas[i + 1];
    }

    (*cont_conta)--;
}

/* procura uma conta pelo NIF e devolve o indice, ou -1 se nao encontrar */
int procurarConta(Conta contas[], int cont_conta, char nif[])
{
    int i;
    for (i = 0; i < cont_conta; i++) {
        if (strcmp(contas[i].nif, nif) == 0) {
            return i;
        }
    }
    return -1;
}

/* pede NIF e senha, devolve 1 se login valido (e preenche nif_logado), 0 caso contrario */
int login(Conta contas[], int cont_conta, char nif_logado[])
{
    char nif[10], senha[21];
    int pos;

    printf("\n---Login---\n");
    printf("NIF: ");
    scanf("%9s", nif);
    printf("Senha: ");
    scanf("%20s", senha);

    pos = procurarConta(contas, cont_conta, nif);

    if (pos == -1) {
        return 0;
    }

    if (strcmp(contas[pos].senha, senha) != 0) {
        return 0;
    }

    strcpy(nif_logado, nif);
    return 1;
}