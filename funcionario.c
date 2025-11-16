#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data.h"

static void print_menu(void) {
    printf("\n--- Funcionário: Sistema Horas Extras ---\n");
    printf("1. Lançar hora extra\n");
    printf("2. Listar meus lançamentos\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

int main(void) {
    load_data();
    int emp_id;
    printf("Seu ID de funcionário: ");
    if (scanf("%d", &emp_id) != 1) { printf("Entrada inválida.\n"); return 0; }
    while (getchar()!='\n');
    if (!find_employee_by_id(emp_id)) { printf("Funcionário não encontrado. Saindo.\n"); return 0; }

    int opt = -1;
    while (1) {
        print_menu();
        if (scanf("%d", &opt) != 1) { while (getchar()!='\n'); printf("Opção inválida.\n"); continue; }
        while (getchar()!='\n');
        if (opt == 0) { printf("Saindo...\n"); break; }
        if (opt == 1) {
            char date[DATE_LEN];
            double hours;
            char reason[REASON_LEN];
            printf("Data (YYYY-MM-DD): ");
            if (!fgets(date, sizeof(date), stdin)) continue;
            date[strcspn(date, "\n")] = 0;
            printf("Horas (ex: 2.5): ");
            if (scanf("%lf", &hours) != 1) { while (getchar()!='\n'); printf("Entrada inválida.\n"); continue; }
            while (getchar()!='\n');
            printf("Motivo: ");
            if (!fgets(reason, sizeof(reason), stdin)) continue;
            reason[strcspn(reason, "\n")] = 0;
            int entry_id = add_overtime_by_employee(emp_id, date, hours, reason);
            if (entry_id < 0) printf("Erro ao criar lançamento.\n");
            else printf("Lançamento criado com ID %d (Pendente).\n", entry_id);
        } else if (opt == 2) {
            list_entries_for_employee(emp_id);
        } else {
            printf("Opção inválida.\n");
        }
    }
    return 0;
}