#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

typedef enum {
    SUCCESS = 0,
    INCORRECT_INPUT_ARGUMENTS,
    INCORRECT_COUNT_ARGUMENTS,
    OVERFLOW_ERROR,
    MEMORY_ALLOCATION_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    FILE_CLOSING_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Некорректный ввод аргументов, попробуйте ещё раз",
        "Некорректный кол-во аргументов, попробуйте ещё раз",
        "Произошло переполнение",
        "Проблемы с выделением памяти",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Ошибка закрытия файла",
        "Неизвестная ошибка, что-то пошло не так"
};

typedef struct Liver {
    char surname[50];
    char firstname[50];
    char patronymic[50];
    int birth_day;
    int birth_month;
    int birth_year;
    char gender;
    double average_income;
    struct Liver* next;
} Liver;

typedef struct Modification {
    char data[20];
    char surname[50];
    char firstname[50];
    char patronymic[50];
    int birth_day;
    int birth_month;
    int birth_year;
    char gender;
    double average_income;

    struct Modification* previous;
    struct Modification* new;
} Modification;

Liver* create_liver() {
    Liver* new_liver = (Liver*)malloc(sizeof(Liver));

    if (new_liver == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }

    new_liver->next = NULL;
    return new_liver;
}

Modification* create_modification() {
    Modification* new_modification = (Modification*)malloc(sizeof(Modification));

    if (new_modification == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }

    new_modification->gender = ' ';
    new_modification->average_income = ' ';
    new_modification->birth_year = 0;
    new_modification->birth_day = 0;
    new_modification->birth_month = 0;
    new_modification->previous = NULL;
    new_modification->new = NULL;

    return new_modification;
}

void print_liver(Liver* liver) {
    printf("\nФамилия: %s", liver->surname);
    printf("\nИмя: %s", liver->firstname);
    printf("\nОтчество: %s", liver->patronymic);
    printf("\nДата рождения: %d.%d.%d", liver->birth_day, liver->birth_month, liver->birth_year);
    printf("\nПол: %c", liver->gender);
    printf("\nСредний доход за месяц: %.2f\n", liver->average_income);
}

Liver* insert_liver(Liver* head, Liver* new_liver) {
    Liver* current = head;
    Liver* prev = NULL;

    while (current != NULL && new_liver->birth_year >= current->birth_year) {
        if (new_liver->birth_year == current->birth_year) {
            if (new_liver->birth_month >= current->birth_month) {
                if (new_liver->birth_month == current->birth_month) {
                    if (new_liver->birth_day >= current->birth_day) {
                        break;
                    }
                } else {
                    break;
                }
            }
        } else if (new_liver->birth_year < current->birth_year) {
            break;
        }

        prev = current;
        current = current->next;
    }

    if (prev == NULL) {
        new_liver->next = head;
        return new_liver;
    } else {
        prev->next = new_liver;
        new_liver->next = current;
        return head;
    }
}

Liver* load_livers_from_file(const char* filepath) {
    FILE* file = fopen(filepath, "r");

    if (file == NULL) {
        printf("%s",errorMessages[FILE_OPENING_ERROR]);
        exit(FILE_OPENING_ERROR);
    }

    Liver* head = NULL;
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
//            free(head);
            exit(FILE_READING_ERROR);
        }
        Liver* new_liver = create_liver();
        sscanf(line, "%s", new_liver->surname);
        fgets(line, sizeof(line), file);
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
//            free(head);
            exit(FILE_READING_ERROR);
        }
        sscanf(line, "%s", new_liver->firstname);
        fgets(line, sizeof(line), file);
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
//            free(head);
            exit(FILE_READING_ERROR);
        }
        sscanf(line, "%s", new_liver->patronymic);
        fgets(line, sizeof(line), file);
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
//            free(head);
            exit(FILE_READING_ERROR);
        }
        sscanf(line, "%d%d%d", &new_liver->birth_day,
               &new_liver->birth_month, &new_liver->birth_year);
        fgets(line, sizeof(line), file);
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
//            free(head);
            exit(FILE_READING_ERROR);
        }
        sscanf(line, "%c", &new_liver->gender);
        fgets(line, sizeof(line), file);
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
//            free(head);
            exit(FILE_READING_ERROR);
        }
        sscanf(line, "%lf", &new_liver->average_income);

        head = insert_liver(head, new_liver);
    }

    if ( fclose (file) == EOF) {
        printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
        exit(FILE_CLOSING_ERROR);
    }
    return head;
}

void save_livers_to_file(const char* filepath, Liver* head) {
    FILE* file = fopen(filepath, "w");

    if (file == NULL) {
        printf("%s",errorMessages[FILE_OPENING_ERROR]);
        exit(FILE_OPENING_ERROR);
    }

    Liver* current = head;

    while (current != NULL) {
        fprintf(file, "Фамилия: %s\n"
                      "Имя: %s\n"
                      "Отчество: %s\n"
                      "Дата рождения: %d.%d.%d\n"
                      "Пол: %c\n"
                      "Средний доход за месяц %.2f\n"
                      "-----------------------------\n",
                current->surname,
                current->firstname,
                current->patronymic,
                current->birth_day,
                current->birth_month,
                current->birth_year,
                current->gender,
                current->average_income
        );

        current = current->next;
    }

    if ( fclose (file) == EOF) {
        printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
        exit(FILE_CLOSING_ERROR);
    }
}

void free_livers(Liver* head) {
    Liver* current = head;
    Liver* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void freeModifications(Modification *head) {
    Modification* current = head;
    Modification* next;

    while (current != NULL) {
        next = current->previous;
        next = next->new;
        free(current);
        current = next;
    }
}

Modification* undo_modifications(Liver** head, const int* modification_count, Modification* root) {
    int undo_count = *modification_count / 2;

    for(int i=0; i<undo_count; i++) {
        if(strcmp("del", root->data) == 0) {
            Liver* current = *head;
            Liver* prev = NULL;

            while (current != NULL) {
                if (strcmp(current->surname, root->surname) == 0) {
                    print_liver(current);

                    if (prev == NULL) *head = current->next;
                    else prev->next = current->next;

                    free(current);
                    break;
                }

                prev = current;
                current = current->next;
            }

        } else if (strcmp("chg", root->data) == 0) {

            Liver* current = *head;

            while (current != NULL) {
                if (strcmp(current->surname, root->surname) == 0) {

                    strcpy(current->surname, root->new->surname);
                    strcpy(current->firstname, root->new->firstname);
                    strcpy(current->patronymic, root->new->patronymic);
                    current->birth_day = root->new->birth_day;
                    current->birth_month = root->new->birth_month;
                    current->birth_year = root->new->birth_year;
                    current->average_income = root->new->average_income;
                    current->gender = root->new->gender;

                }

                current = current->next;
            }

        } else if (strcmp("add", root->data) == 0) {
            Liver* new_liver = create_liver();

            strcpy(new_liver->surname, root->surname);
            strcpy(new_liver->firstname, root->firstname);
            strcpy(new_liver->patronymic, root->patronymic);
            new_liver->birth_day = root->birth_day;
            new_liver->birth_month = root->birth_month;
            new_liver->birth_year = root->birth_year;
            new_liver->average_income = root->average_income;
            new_liver->gender = root->gender;

            *head = insert_liver(*head, new_liver);
        }

        root = root->previous;
    }

//    free(root);
    return root = create_modification();
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    Liver* head = NULL;
    Modification* root = create_modification();
    Modification* extra = NULL;

    int modification_count = 0;

    char filepath[256];
    printf("Введите путь к файлу: ");
    fgets(filepath, sizeof(filepath), stdin);
    filepath[strcspn(filepath, "\n")] = 0;

    head = load_livers_from_file(filepath);

    int choice = -1;

    while (choice != 0) {
        choice = -1;
        printf("\n1 - Добавить жителя");
        printf("\n2 - Найти жителя");
        printf("\n3 - Изменить информацию о жителе");
        printf("\n4 - Удалить информацию о жителе");
        printf("\n5 - Сохранить данные в файл");
        printf("\n6 - Отменить последние изменения");
        printf("\n0 - Выход");

        printf("\n\nВыберите действие: ");
        scanf("%d", &choice);
        getchar();

//        choice = 1;


        if (choice == 1) {
            Liver* new_liver = create_liver();

            extra = create_modification();
            Modification *thirdAssignmentOperator;
            thirdAssignmentOperator = root;
            root = extra;
            root->previous = thirdAssignmentOperator;


            strcpy(root->data, "del");

            printf("\nВведите фамилию: ");
            fgets(new_liver->surname, sizeof(new_liver->surname), stdin);
            new_liver->surname[strcspn(new_liver->surname, "\n")] = 0;
            strcpy(root->surname, new_liver->surname);

            printf("Введите имя: ");
            fgets(new_liver->firstname, sizeof(new_liver->firstname), stdin);
            new_liver->firstname[strcspn(new_liver->firstname, "\n")] = 0;
            strcpy(root->firstname, new_liver->firstname);

            printf("Введите отчество: ");
            fgets(new_liver->patronymic, sizeof(new_liver->patronymic), stdin);
            new_liver->patronymic[strcspn(new_liver->patronymic, "\n")] = 0;
            strcpy(root->patronymic, new_liver->patronymic);

            printf("Введите дату рождения (число, месяц, год): ");
            scanf("%d%d%d", &new_liver->birth_day, &new_liver->birth_month, &new_liver->birth_year);
            root->birth_month = new_liver->birth_month;
            root->birth_day = new_liver->birth_day;
            root->birth_year = new_liver->birth_year;
            getchar();

            printf("Введите пол (M - мужской, W - женский): ");
            scanf("%c", &new_liver->gender);
            root->gender = new_liver->gender;

            getchar();

            printf("Введите средний доход за месяц: ");
            scanf("%lf", &new_liver->average_income);
            root->average_income = new_liver->average_income;
            getchar();

            head = insert_liver(head, new_liver);
            modification_count++;
//            exit(1);
        } else if (choice == 2) {
            char search_surname[50];
            printf("Введите фамилию для поиска: ");
            fgets(search_surname, sizeof(search_surname), stdin);
            search_surname[strcspn(search_surname, "\n")] = 0;

            Liver* current = head;
            int found = 0;

            while (current != NULL) {
                if (strcmp(current->surname, search_surname) == 0) {
                    print_liver(current);
                    found = 1;
                }

                current = current->next;
            }

            if (!found) {
                printf("\nЖитель с фамилией %s не найден.", search_surname);
            }
        } else if (choice == 3) {

            extra = create_modification();
            Modification *thirdAssignmentOperator;
            thirdAssignmentOperator = root;
            root = extra;
            root->previous = thirdAssignmentOperator;

            strcpy(root->data, "chg");

            char search_surname[50];
            printf("Введите фамилию для изменения: ");
            fgets(search_surname, sizeof(search_surname), stdin);
            search_surname[strcspn(search_surname, "\n")] = 0;
            Liver* current = head;
            int found = 0;

            while (current != NULL) {
                if (strcmp(current->surname, search_surname) == 0) {

                    if(root->new == NULL) root->new = create_modification();

                    strcpy(root->new->surname, current->surname);
                    strcpy(root->new->firstname, current->firstname);
                    strcpy(root->new->patronymic, current->patronymic);
                    root->new->birth_day = current->birth_day;
                    root->new->birth_month = current->birth_month;
                    root->new->birth_year = current->birth_year;
                    root->new->average_income = current->average_income;
                    root->new->gender = current->gender;

                    print_liver(current);

                    printf("\nВведите новое значение фамилии: ");
                    fgets(current->surname, sizeof(current->surname), stdin);
                    current->surname[strcspn(current->surname, "\n")] = 0;
                    strcpy(root->surname, current->surname);

                    printf("Введите новое значение имени: ");
                    fgets(current->firstname, sizeof(current->firstname), stdin);
                    current->firstname[strcspn(current->firstname, "\n")] = 0;
                    strcpy(root->firstname, current->firstname);

                    printf("Введите новое значение отчества: ");
                    fgets(current->patronymic, sizeof(current->patronymic), stdin);
                    current->patronymic[strcspn(current->patronymic, "\n")] = 0;
                    strcpy(root->patronymic, current->patronymic);

                    printf("Введите новую дату рождения (число, месяц, год): ");
                    scanf("%d%d%d", &current->birth_day, &current->birth_month, &current->birth_year);
                    root->birth_day = current->birth_day;
                    root->birth_month = current->birth_month;
                    root->birth_year = current->birth_year;
                    getchar();

                    printf("Введите новое значение пола (M - мужской, W - женский): ");
                    scanf("%c", &current->gender);
                    root->gender = current->gender;
                    getchar();

                    printf("Введите новое значение среднего дохода за месяц: ");
                    scanf("%lf", &current->average_income);
                    root->average_income = current->average_income;
                    getchar();

                    found = 1;
                }

                current = current->next;
            }

            modification_count++;

            if (!found) {
                printf("\nЖитель с фамилией %s не найден.", search_surname);
            }

        } else if (choice == 4) {

            extra = create_modification();
            Modification *thirdAssignmentOperator;
            thirdAssignmentOperator = root;
            root = extra;
            root->previous = thirdAssignmentOperator;

            strcpy(root->data, "add");

            char search_surname[50];
            printf("Введите фамилию для удаления: ");
            fgets(search_surname, sizeof(search_surname), stdin);
            search_surname[strcspn(search_surname, "\n")] = 0;

            Liver* current = head;
            Liver* prev = NULL;
            int found = 0;

            while (current != NULL) {
                if (strcmp(current->surname, search_surname) == 0) {

                    strcpy(root->surname, current->surname);
                    strcpy(root->firstname, current->firstname);
                    strcpy(root->patronymic, current->patronymic);
                    root->birth_day = current->birth_day;
                    root->birth_month = current->birth_month;
                    root->birth_year = current->birth_year;
                    root->average_income = current->average_income;
                    root->gender = current->gender;

                    print_liver(current);

                    if (prev == NULL) {
                        head = current->next;
                    } else {
                        prev->next = current->next;
                    }

                    free(current);
                    modification_count++;
                    found = 1;
                    break;
                }

                prev = current;
                current = current->next;
            }

            if (!found) {
                printf("\nЖитель с фамилией %s не найден.", search_surname);
            }
        } else if (choice == 5) {
            char save_filepath[256];
            printf("Введите путь для сохранения файла: ");
            fgets(save_filepath, sizeof(save_filepath), stdin);
            save_filepath[strcspn(save_filepath, "\n")] = 0;

            save_livers_to_file(save_filepath, head);
            printf("\nДанные сохранены в файл %s", save_filepath);
        } else if (choice == 6) {
            undo_modifications(&head, &modification_count, root);
        }
    }

    free_livers(head);
    freeModifications(root);

    return SUCCESS;
}
