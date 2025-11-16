#include "data.h"
#include <string.h>
#include <stdlib.h>

static Employee employees[MAX_EMP];
static Overtime entries[MAX_ENT];
static int emp_count = 0;
static int entry_count = 0;
static int next_emp_id = 1;
static int next_entry_id = 1;

static const char *status_str(Status s) {
    switch (s) {
        case PENDING: return "Pendente";
        case APPROVED: return "Aprovado";
        case REJECTED: return "Rejeitado";
        default: return "Desconhecido";
    }
}

/* limpa a tela (usa cls no Windows, clear em Unix) */
void clear_console(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void save_data(void) {
    FILE *f = fopen(DATA_FILE, "w");
    if (!f) return;
    for (int i = 0; i < emp_count; ++i) {
        fprintf(f, "E;%d;%s\n", employees[i].id, employees[i].name);
    }
    for (int i = 0; i < entry_count; ++i) {
        fprintf(f, "O;%d;%d;%s;%.2f;%s;%d\n",
                entries[i].id,
                entries[i].emp_id,
                entries[i].date,
                entries[i].hours,
                entries[i].reason,
                (int)entries[i].status);
    }
    fclose(f);
}

void load_data(void) {
    FILE *f = fopen(DATA_FILE, "r");
    if (!f) return;
    char line[512];
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == 'E' && line[1] == ';') {
            int id;
            char name[NAME_LEN];
            if (sscanf(line+2, "%d;%63[^\n]", &id, name) >= 1) {
                employees[emp_count].id = id;
                strncpy(employees[emp_count].name, name, NAME_LEN-1);
                employees[emp_count].name[NAME_LEN-1] = '\0';
                emp_count++;
                if (id >= next_emp_id) next_emp_id = id + 1;
            }
        } else if (line[0] == 'O' && line[1] == ';') {
            Overtime o;
            int st;
            char reason_tmp[REASON_LEN];
            if (sscanf(line+2, "%d;%d;%15[^;];%lf;%127[^;];%d",
                       &o.id, &o.emp_id, o.date, &o.hours, reason_tmp, &st) >= 5) {
                strncpy(o.reason, reason_tmp, REASON_LEN-1);
                o.reason[REASON_LEN-1] = '\0';
                o.status = (Status)st;
                entries[entry_count++] = o;
                if (o.id >= next_entry_id) next_entry_id = o.id + 1;
            }
        }
    }
    fclose(f);
}

Employee *find_employee_by_id(int id) {
    for (int i = 0; i < emp_count; ++i)
        if (employees[i].id == id) return &employees[i];
    return NULL;
}

/* manager functions */

int add_employee_manager(const char *name) {
    if (emp_count >= MAX_EMP) return -1;
    if (!name || name[0] == '\0') return -1;
    employees[emp_count].id = next_emp_id++;
    strncpy(employees[emp_count].name, name, NAME_LEN-1);
    employees[emp_count].name[NAME_LEN-1] = '\0';
    emp_count++;
    save_data();
    return next_emp_id - 1;
}

void list_employees_manager(void) {
    if (emp_count == 0) {
        printf("Nenhum funcionário cadastrado.\n");
        return;
    }
    printf("ID\tNome\n");
    for (int i = 0; i < emp_count; ++i) {
        printf("%d\t%s\n", employees[i].id, employees[i].name);
    }
}

void list_all_entries(void) {
    if (entry_count == 0) { printf("Nenhum lançamento.\n"); return; }
    printf("ID\tEmpID\tData\t\tHoras\tStatus\tMotivo\n");
    for (int i = 0; i < entry_count; ++i) {
        printf("%d\t%d\t%s\t%.2f\t%s\t%s\n",
               entries[i].id,
               entries[i].emp_id,
               entries[i].date,
               entries[i].hours,
               status_str(entries[i].status),
               entries[i].reason);
    }
}

int approve_reject_entry_manager(int entry_id, int approve) {
    for (int i = 0; i < entry_count; ++i) {
        if (entries[i].id == entry_id) {
            entries[i].status = approve ? APPROVED : REJECTED;
            save_data();
            return 0;
        }
    }
    return -1;
}

void total_hours_report_manager(void) {
    if (emp_count == 0) { printf("Nenhum funcionário cadastrado.\n"); return; }
    printf("Relatório de horas aprovadas por funcionário:\n");
    for (int i = 0; i < emp_count; ++i) {
        double total = 0.0;
        for (int j = 0; j < entry_count; ++j) {
            if (entries[j].emp_id == employees[i].id && entries[j].status == APPROVED)
                total += entries[j].hours;
        }
        printf("%d - %s : %.2f horas\n", employees[i].id, employees[i].name, total);
    }
}

/* employee API (only allowed to employee app) */

int add_overtime_by_employee(int emp_id, const char *date, double hours, const char *reason) {
    if (entry_count >= MAX_ENT) return -1;
    if (!find_employee_by_id(emp_id)) return -1;
    Overtime o;
    o.id = next_entry_id++;
    o.emp_id = emp_id;
    strncpy(o.date, date ? date : "", DATE_LEN-1); o.date[DATE_LEN-1] = '\0';
    o.hours = hours;
    strncpy(o.reason, reason ? reason : "", REASON_LEN-1); o.reason[REASON_LEN-1] = '\0';
    o.status = PENDING;
    entries[entry_count++] = o;
    save_data();
    return o.id;
}

void list_entries_for_employee(int emp_id) {
    int found = 0;
    printf("ID\tData\t\tHoras\tStatus\tMotivo\n");
    for (int i = 0; i < entry_count; ++i) {
        if (entries[i].emp_id == emp_id) {
            printf("%d\t%s\t%.2f\t%s\t%s\n",
                   entries[i].id,
                   entries[i].date,
                   entries[i].hours,
                   status_str(entries[i].status),
                   entries[i].reason);
            found = 1;
        }
    }
    if (!found) printf("Nenhum lançamento para o funcionário %d.\n", emp_id);
}