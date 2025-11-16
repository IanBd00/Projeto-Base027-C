#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

static void print_menu(void) {
    printf("\n--- Gerente: Sistema Horas Extras ---\n");
    printf("1. Cadastrar funcionário\n");
    printf("2. Listar funcionários\n");
    printf("3. Listar todos os lançamentos\n");
    printf("4. Aprovar lançamento\n");
    printf("5. Rejeitar lançamento\n");
    printf("6. Relatório total de horas aprovadas\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

int main(void) {
    load_data();
    /* simples autenticação */
    char pwd[64];
    printf("Senha do gerente: ");
    if (!fgets(pwd, sizeof(pwd), stdin)) return 0;
    pwd[strcspn(pwd, "\n")] = 0;
    if (strcmp(pwd, "admin123") != 0) {
        printf("Senha incorreta. Saindo.\n");
        return 0;
    }

    int opt = -1;
    while (1) {
        print_menu();
        if (scanf("%d", &opt) != 1) { while (getchar()!='\n'); printf("Opção inválida.\n"); continue; }
        while (getchar()!='\n');
        if (opt == 0) { printf("Saindo...\n"); break; }
        switch (opt) {
            case 1: {
                char name[NAME_LEN];
                printf("Nome do funcionário: ");
                if (!fgets(name, sizeof(name), stdin)) break;
                name[strcspn(name, "\n")] = 0;
                int id = add_employee_manager(name);
                if (id < 0) printf("Erro ao cadastrar.\n"); else printf("Funcionário cadastrado com ID %d.\n", id);
                break;
            }
            case 2: list_employees_manager(); break;
            case 3: list_all_entries(); break;
            case 4:
            case 5: {
                int id;
                printf("ID do lançamento: ");
                if (scanf("%d", &id) != 1) { while (getchar()!='\n'); printf("Entrada inválida.\n"); break; }
                while (getchar()!='\n');
                if (approve_reject_entry_manager(id, opt == 4) == 0)
                    printf("Lançamento %d atualizado.\n", id);
                else
                    printf("Lançamento não encontrado.\n");
                break;
            }
            case 6: total_hours_report_manager(); break;
            default: printf("Opção inválida.\n"); break;
        }
    }
    return 0;
}