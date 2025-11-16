#ifndef DATA_H
#define DATA_H

#include <stdio.h>

#define MAX_EMP 200
#define MAX_ENT 2000
#define NAME_LEN 64
#define REASON_LEN 128
#define DATE_LEN 16
#define DATA_FILE "data.csv"

typedef enum { PENDING = 0, APPROVED = 1, REJECTED = 2 } Status;

typedef struct {
    int id;
    char name[NAME_LEN];
} Employee;

typedef struct {
    int id;
    int emp_id;
    char date[DATE_LEN]; // formato dd-mm-yyyy
    double hours;
    char reason[REASON_LEN];
    Status status;
} Overtime;

/* basic load/save */
void load_data(void);
void save_data(void);

/* shared queries */
Employee *find_employee_by_id(int id);

/* helper */
void clear_console(void);

/* employee API (allowed to employee.exe) */
int add_overtime_by_employee(int emp_id, const char *date, double hours, const char *reason);
void list_entries_for_employee(int emp_id);

/* manager API (only used by manager.exe) */
int add_employee_manager(const char *name); /* returns new emp id or -1 */
void list_employees_manager(void);
void list_all_entries(void);
int approve_reject_entry_manager(int entry_id, int approve); /* returns 0 ok, -1 not found */
void total_hours_report_manager(void);

#endif /* DATA_H */