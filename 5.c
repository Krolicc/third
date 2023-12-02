#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

typedef enum {
    SUCCESS = 0,
    INCORRECT_INPUT,
    INCORRECT_ARGUMENTS_COUNT,
    OVERFLOW_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    MEMORY_ALLOCATION_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Некорректный ввод, попробуйте ещё раз",
        "Неккоректное кол-во данных",
        "Произошло переполнение",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Ошибка выделения памяти",
        "Неизвестная ошибка, что-то пошло не так"
};

typedef struct {
    int id;
    char* name;
    char* surname;
    char* group;
    unsigned char* grades;
} Student;

void freeStudent(Student* student) {
    free(student->name);
    free(student->surname);
    free(student->group);
    free(student->grades);
}

int compareById(const void* a, const void* b) {
    const Student* studentA = (const Student*)a;
    const Student* studentB = (const Student*)b;
    return studentA->id - studentB->id;
}

int compareBySurname(const void* a, const void* b) {
    const Student* studentA = (const Student*)a;
    const Student* studentB = (const Student*)b;
    return strcmp(studentA->surname, studentB->surname);
}

int compareByName(const void* a, const void* b) {
    const Student* studentA = (const Student*)a;
    const Student* studentB = (const Student*)b;
    return strcmp(studentA->name, studentB->name);
}

int compareByGroup(const void* a, const void* b) {
    const Student* studentA = (const Student*)a;
    const Student* studentB = (const Student*)b;
    return strcmp(studentA->group, studentB->group);
}

void printStudent(const Student* student) {
    printf("\nID: %d\n", student->id);
    printf("Имя: %s\n", student->name);
    printf("Фамилия: %s\n", student->surname);
    printf("Группа: %s\n", student->group);

    double averageGrade = 0;
    for (int i = 0; i < 5; i++) {
        averageGrade += student->grades[i];
    }
    averageGrade /= 5;
    printf("Средняя оценка: %.2f\n", averageGrade);
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc != 3) {
        printf("%s", errorMessages[INCORRECT_ARGUMENTS_COUNT]);
        return INCORRECT_ARGUMENTS_COUNT;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        printf("%s", errorMessages[FILE_OPENING_ERROR]);
        return FILE_OPENING_ERROR;
    }

    int count = 0;
    Student* students = NULL;

    while (!feof(file)) {
        Student student;
        fscanf(file, "%d ", &student.id);

        int counter = 0, j = 0, k = -1;
        char fullInfo[1024];
        char name[100];
        char surname[100];
        char group[100];
        char gradesStr[20] = "";

        fgets(fullInfo, sizeof(name), file);

        for(int i=0; i<strlen(fullInfo); i++) {
            if(fullInfo[i] == ' ') {
                counter++;
                j=0;
                if (counter < 3) continue;
            }

            switch (counter) {
                case 0:
                    name[j] = fullInfo[i];
                    j++;
                    name[j] = '\0';
                    break;
                case 1:
                    surname[j] = fullInfo[i];
                    j++;
                    surname[j] = '\0';
                    break;
                case 2:
                    group[j] = fullInfo[i];
                    j++;
                    group[j] = '\0';
                    break;
                default:
                    if(fullInfo[i] == ' ' && k == -1) break;
                    k++;
                    gradesStr[k] = fullInfo[i];
                    break;
            }
        }

        student.name = strdup(name);
        student.surname = strdup(surname);
        student.group = strdup(group);
        student.grades = (unsigned char*)malloc(5 * sizeof(unsigned char));

        if (student.grades == NULL) {
            printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
            fclose(file);
            return MEMORY_ALLOCATION_ERROR;
        }

        sscanf(gradesStr, "%hhu%hhu%hhu%hhu%hhu", &student.grades[0],  &student.grades[1],
               &student.grades[2], &student.grades[3], &student.grades[4]);

        students = (Student*)realloc(students, (count + 1) * sizeof(Student));
        students[count] = student;
        count++;
    }

    fclose(file);

    char choice[3];
    printf("Выберите опцию:\n");
    printf("0. Поиск по ID\n");
    printf("1. Поиск по Фамилии\n");
    printf("2. Поиск по Имени\n");
    printf("3. Поиск по Группе\n");
    printf("4. Сортировка по ID\n");
    printf("5. Сортировка по Фамилии\n");
    printf("6. Сортировка по Имени\n");
    printf("7. Сортировка по Группе\n");
    printf("8. Вывод студента по ID в трассировочный файл\n");
    printf("9. Вывод студента с оценками выше среднего в трассировочный файл\n");
    printf("Для выхода введите -1\n");

//    char traceFilePath[100];
//    printf("Enter trace file path: ");
//    fgets(traceFilePath, sizeof(traceFilePath), stdin);
//    traceFilePath[strcspn(traceFilePath, "\n")] = '\0';

    FILE* traceFile = fopen(argv[2], "w");

    if (!traceFile) {
        printf("%s", errorMessages[FILE_OPENING_ERROR]);
        return FILE_OPENING_ERROR;
    }
    int numChoice;

    do {
        printf("Выберите один из вариантов: ");
        scanf("%s", choice);
        if(strchr("-0123456789", choice[0]) == NULL) {
            printf("Неверный выбор.\n");
            continue;
        }

        numChoice = atoi(choice);

        switch (numChoice) {
            case 0: {
                int searchId;
                printf("Введите ID: ");
                scanf("%d", &searchId);

                for (int i = 0; i < count; i++) {
                    if (students[i].id == searchId) {
                        printStudent(&students[i]);
                    }
                }
                break;
            }
            case 1: {
                char searchSurname[100];
                printf("Введите фамилию: ");
                scanf("%s", searchSurname);

                for (int i = 0; i < count; i++) {
                    if (strcmp(students[i].surname, searchSurname) == 0) {
                        printStudent(&students[i]);
                    }
                }
                break;
            }
            case 2: {
                char searchName[100];
                printf("Введите имя: ");
                scanf("%s", searchName);

                for (int i = 0; i < count; i++) {
                    if (strcmp(students[i].name, searchName) == 0) {
                        printStudent(&students[i]);
                    }
                }
                break;
            }
            case 3: {
                char searchGroup[100];
                printf("Введите группу: ");
                scanf("%s", searchGroup);

                for (int i = 0; i < count; i++) {
                    if (strcmp(students[i].group, searchGroup) == 0) {
                        printStudent(&students[i]);
                    }
                }
                break;
            }
            case 4:
                qsort(students, count, sizeof(Student), compareById);
                for (int i = 0; i < count; i++)
                    printStudent(&students[i]);
                break;
            case 5:
                qsort(students, count, sizeof(Student), compareBySurname);
                for (int i = 0; i < count; i++)
                    printStudent(&students[i]);
                break;
            case 6:
                qsort(students, count, sizeof(Student), compareByName);
                for (int i = 0; i < count; i++)
                    printStudent(&students[i]);
                break;
            case 7:
                qsort(students, count, sizeof(Student), compareByGroup);
                for (int i = 0; i < count; i++)
                    printStudent(&students[i]);
                break;
            case 8: {
                int searchId;
                printf("Введите ID: ");
                scanf("%d", &searchId);

                for (int i = 0; i < count; i++) {
                    if (students[i].id == searchId) {
                        fprintf(traceFile, "ID: %d\n", students[i].id);
                        fprintf(traceFile, "Имя: %s\n", students[i].name);
                        fprintf(traceFile, "Фамилия: %s\n", students[i].surname);
                        fprintf(traceFile, "Группа: %s\n", students[i].group);

                        double averageGrade = 0;
                        for (int j = 0; j < 5; j++) {
                            averageGrade += students[i].grades[j];
                        }
                        averageGrade /= 5;
                        fprintf(traceFile, "Средняя оценка: %.2f\n", averageGrade);

                        break;
                    }
                }
                break;
            }
            case 9: {
                double averageGradeSum = 0;
                int averageGradeCount = 0;
                for (int i = 0; i < count; i++) {
                    double averageGrade = 0;
                    for (int j = 0; j < 5; j++) {
                        averageGrade += students[i].grades[j];
                    }
                    averageGrade /= 5;

                    averageGradeSum += averageGrade;
                    averageGradeCount++;
                }

                double averageGradeAverage = averageGradeSum / averageGradeCount;
                fprintf(traceFile, "Учащиеся с оценками выше среднего:\n");

                for (int i = 0; i < count; i++) {
                    double averageGrade = 0;
                    for (int j = 0; j < 5; j++) {
                        averageGrade += students[i].grades[j];
                    }
                    averageGrade /= 5;

                    if (averageGrade > averageGradeAverage) {
                        fprintf(traceFile, "Имя: %s\n", students[i].name);
                        fprintf(traceFile, "Фамилия: %s\n", students[i].surname);
                    }
                }
                break;
            }
            case -1:
                break;
            default:
                printf("Неверный выбор.\n");
                break;
        }
    } while (numChoice != -1);

    fclose(traceFile);

    for (int i = 0; i < count; i++) {
        freeStudent(&students[i]);
    }
    free(students);

    return SUCCESS;
}
