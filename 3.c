#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (argc < 4) {
        printf("Usage: program_name input_file flag output_file\n");
        return 1;
    }

    FILE *inputFile = fopen(argv[1], "r");
    if (inputFile == NULL) {
        printf("Error opening input file.\n");
        return 1;
    }

    char flag = argv[2][1];

    FILE *outputFile = fopen(argv[3], "w");
    if (outputFile == NULL) {
        printf("Error opening output file.\n");
        fclose(inputFile);
        return 1;
    }

    int numEmployees = 0;
    fscanf(inputFile, "%d", &numEmployees);

    Employee *employees = malloc(numEmployees * sizeof(Employee));
    if (employees == NULL) {
        printf("Error allocating memory.\n");
        fclose(inputFile);
        fclose(outputFile);
        return 1;
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
    fclose(inputFile);
    fclose(outputFile);

    return 0;
}
