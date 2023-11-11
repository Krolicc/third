#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Student {
    unsigned int id;
    char* name;
    char* surname;
    char* group;
    unsigned char* marks;
};

int compareById(const void* a, const void* b) {
    struct Student* studentA = (struct Student*)a;
    struct Student* studentB = (struct Student*)b;

    return (studentA->id - studentB->id);
}

int compareBySurname(const void* a, const void* b) {
    struct Student* studentA = (struct Student*)a;
    struct Student* studentB = (struct Student*)b;

    return strcmp(studentA->surname, studentB->surname);
}

int compareByName(const void* a, const void* b) {
    struct Student* studentA = (struct Student*)a;
    struct Student* studentB = (struct Student*)b;

    return strcmp(studentA->name, studentB->name);
}

int compareByGroup(const void* a, const void* b) {
    struct Student* studentA = (struct Student*)a;
    struct Student* studentB = (struct Student*)b;

    return strcmp(studentA->group, studentB->group);
}

void printStudent(struct Student* student) {
    printf("ID: %u\n", student->id);
    printf("Фамилия: %s\n", student->surname);
    printf("Имя: %s\n", student->name);
    printf("Группа: %s\n", student->group);

    unsigned int sum = 0;
    for (int i = 0; i < 5; ++i) {
        sum += student->marks[i];
    }
    double average = (double)sum / 5.0;

    printf("Средняя оценка: %.2f\n", average);
}

void printAboveAverage(struct Student* students, int count, double average) {
    for (int i = 0; i < count; ++i) {
        unsigned int sum = 0;
        for (int j = 0; j < 5; ++j) {
            sum += students[i].marks[j];
        }
        double studentAverage = (double)sum / 5.0;

        if (studentAverage > average) {
            printf("Фамилия: %s, Имя: %s\n", students[i].surname, students[i].name);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Укажите путь к файлу\n");
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("Ошибка при открытии файла\n");
        return 1;
    }

    struct Student* students = NULL;
    int count = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        struct Student newStudent;

        char* token = strtok(line, ",");
        newStudent.id = atoi(token);

        token = strtok(NULL, ",");
        newStudent.name = strdup(token);

        token = strtok(NULL, ",");
        newStudent.surname = strdup(token);

        token = strtok(NULL, ",");
        newStudent.group = strdup(token);

        token = strtok(NULL, ",");
        newStudent.marks = malloc(sizeof(unsigned char) * 5);
        for (int i = 0; i < 5; ++i) {
            newStudent.marks[i] = atoi(strtok(token, " "));
            token = strtok(NULL, ",");
        }

        students = realloc(students, sizeof(struct Student) * (count + 1));
        students[count] = newStudent;
        count++;
    }

    fclose(file);

    int option;
    double average = 0.0;

    while (1) {
        printf("1. Поиск студента по ID\n");
        printf("2. Поиск студента по фамилии\n");
        printf("3. Поиск студента по имени\n");
        printf("4. Поиск студента по группе\n");
        printf("5. Сортировка студентов по ID\n");
        printf("6. Сортировка студентов по фамилии\n");
        printf("7. Сортировка студентов по имени\n");
        printf("8. Сортировка студентов по группе\n");
        printf("9. Вывод данных найденного студента по ID в трассировочный файл\n");
        printf("10. Вывод фамилий и имен студентов с выше средним баллом в трассировочный файл\n");
        printf("0. Выход\n");

        printf("Выберите опцию: ");
        scanf("%d", &option);

        if (option == 0) {
            break;
        }

        switch (option) {
            case 1: {
                unsigned int id;
                printf("Введите ID студента: ");
                scanf("%u", &id);

                int found = 0;
                for (int i = 0; i < count; ++i) {
                    if (students[i].id == id) {
                        printStudent(&students[i]);
                        found = 1;
                        break;
                    }
                }

                if (!found) {
                    printf("Студент с ID %u не найден\n", id);
                }
                break;
            }
            case 2: {
                char surname[256];
                printf("Введите фамилию студента: ");
                scanf("%s", surname);

                int found = 0;
                for (int i = 0; i < count; ++i) {
                    if (strcmp(students[i].surname, surname) == 0) {
                        printStudent(&students[i]);
                        found = 1;
                    }
                }

                if (!found) {
                    printf("Студент с фамилией %s не найден\n", surname);
                }
                break;
            }
            case 3: {
                char name[256];
                printf("Введите имя студента: ");
                scanf("%s", name);

                int found = 0;
                for (int i = 0; i < count; ++i) {
                    if (strcmp(students[i].name, name) == 0) {
                        printStudent(&students[i]);
                        found = 1;
                    }
                }

                if (!found) {
                    printf("Студент с именем %s не найден\n", name);
                }
                break;
            }
            case 4: {
                char group[256];
                printf("Введите группу студента: ");
                scanf("%s", group);

                int found = 0;
                for (int i = 0; i < count; ++i) {
                    if (strcmp(students[i].group, group) == 0) {
                        printStudent(&students[i]);
                        found = 1;
                    }
                }

                if (!found) {
                    printf("Студенты в группе %s не найдены\n", group);
                }
                break;
            }
            case 5: {
                qsort(students, count, sizeof(struct Student), compareById);
                printf("Коллекция студентов успешно отсортирована по ID\n");
                break;
            }
            case 6: {
                qsort(students, count, sizeof(struct Student), compareBySurname);
                printf("Коллекция студентов успешно отсортирована по фамилии\n");
                break;
            }
            case 7: {
                qsort(students, count, sizeof(struct Student), compareByName);
                printf("Коллекция студентов успешно отсортирована по имени\n");
                break;
            }
            case 8: {
                qsort(students, count, sizeof(struct Student), compareByGroup);
                printf("Коллекция студ