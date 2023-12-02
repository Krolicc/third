#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

typedef enum {
    SUCCESS = 0,
    INCORRECT_INPUT,
    INCORRECT_ARGUMENTS_COUNT,
    OVERFLOW_ERROR,
    MALLOC_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    FILE_CLOSING_ERROR,
    MEMORY_ALLOCATION_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Некорректный ввод, попробуйте ещё раз",
        "Неккоректное кол-во данных",
        "Произошло переполнение",
        "Проблемы с выделением памяти",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Не удалось закрыть файл",
        "Ошибка выделения памяти",
        "Неизвестная ошибка, что-то пошло не так"
};

typedef struct {
    int id;
    char firstName[50];
    char lastName[50];
    float salary;
} Employee;

int compareEmployees(const void *a, const void *b) {
    Employee *empA = (Employee *)a;
    Employee *empB = (Employee *)b;

    if (empA->salary != empB->salary) {
        return (empA->salary > empB->salary) ? 1 : -1;
    }
    if (strcmp(empA->lastName, empB->lastName) != 0) {
        return strcmp(empA->lastName, empB->lastName);
    }
    if (strcmp(empA->firstName, empB->firstName) != 0) {
        return strcmp(empA->firstName, empB->firstName);
    }
    return (empA->id > empB->id) ? 1 : -1;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("%s", errorMessages[INCORRECT_ARGUMENTS_COUNT]);
        return INCORRECT_ARGUMENTS_COUNT;
    }

    FILE *inputFile = fopen(argv[1], "r");

    if (inputFile == NULL) {
        printf("%s", errorMessages[FILE_OPENING_ERROR]);
        return FILE_OPENING_ERROR;
    }

    if (strchr("-/",argv[2][0]) != NULL && strchr("da", argv[2][1]) != NULL) {
        char flag = argv[2][1];
    }

    FILE *outputFile = fopen(argv[3], "w");
    if (outputFile == NULL) {
        printf("%s", errorMessages[FILE_OPENING_ERROR]);
        fclose(inputFile);
        return FILE_OPENING_ERROR;
    }

    int numEmployees = 0;
    fscanf(inputFile, "%d", &numEmployees);

    Employee *employees = malloc(numEmployees * sizeof(Employee));

    if (employees == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        fclose(inputFile);
        fclose(outputFile);
        return MEMORY_ALLOCATION_ERROR;
    }

    for (int i = 0; i < numEmployees; i++) {
        fscanf(inputFile, "%d %s %s %f",
                &employees[i].id,
                employees[i].firstName,
                employees[i].lastName,
                &employees[i].salary);
    }

    qsort(employees, numEmployees, sizeof(Employee), compareEmployees);

    for (int i = 0; i < numEmployees; i++) {
        fprintf(outputFile, "%d %s %s %.2f\n",
                employees[i].id,
                employees[i].firstName,
                employees[i].lastName,
                employees[i].salary);
    }

    free(employees);

    if(fclose(inputFile)==EOF) {
        perror(errorMessages[FILE_CLOSING_ERROR]);
        return FILE_CLOSING_ERROR;
    }

    if(fclose(outputFile)==EOF) {
        perror(errorMessages[FILE_CLOSING_ERROR]);
        return FILE_CLOSING_ERROR;
    }

    return SUCCESS;
}
