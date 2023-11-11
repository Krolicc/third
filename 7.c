#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LEN 100

typedef struct Date {
    int day;
    int month;
    int year;
} Date;

typedef struct Liver {
    char surname[MAX_NAME_LEN];
    char name[MAX_NAME_LEN];
    char middleName[MAX_NAME_LEN];
    Date birthDate;
    char gender;
    double averageIncome;
    struct Liver* next;
} Liver;

int getModificationCount(Liver* head) {
    int count = 0;
    Liver* curr = head;
    while (curr != NULL) {
        count++;
        curr = curr->next;
    }
    return count;
}

void addLiver(Liver** head, char* surname, char* name, char* middleName, Date birthDate, char gender, double averageIncome) {
    Liver* newLiver = (Liver*)malloc(sizeof(Liver));
    strcpy(newLiver->surname, surname);
    strcpy(newLiver->name, name);
    strcpy(newLiver->middleName, middleName);
    newLiver->birthDate = birthDate;
    newLiver->gender = gender;
    newLiver->averageIncome = averageIncome;
    newLiver->next = NULL;

    if (*head == NULL) {
        *head = newLiver;
    } else if (birthDate.year < (*head)->birthDate.year) {
        newLiver->next = *head;
        *head = newLiver;
    } else {
        Liver* curr = *head;
        while (curr->next != NULL && curr->next->birthDate.year <= birthDate.year) {
            curr = curr->next;
        }
        newLiver->next = curr->next;
        curr->next = newLiver;
    }
}

Liver* findLiver(Liver* head, char* surname, char* name, char* middleName, Date birthDate, char gender, double averageIncome) {
    Liver* curr = head;
    while (curr != NULL) {
        if (strcmp(curr->surname, surname) == 0 && strcmp(curr->name, name) == 0 && strcmp(curr->middleName, middleName) == 0
            && curr->birthDate.day == birthDate.day && curr->birthDate.month == birthDate.month && curr->birthDate.year == birthDate.year
            && curr->gender == gender && curr->averageIncome == averageIncome) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void deleteLiver(Liver** head, char* surname, char* name, char* middleName, Date birthDate, char gender, double averageIncome) {
    if (*head == NULL) {
        return;
    }

    Liver* temp = *head;
    if (strcmp(temp->surname, surname) == 0 && strcmp(temp->name, name) == 0 && strcmp(temp->middleName, middleName) == 0
        && temp->birthDate.day == birthDate.day && temp->birthDate.month == birthDate.month && temp->birthDate.year == birthDate.year
        && temp->gender == gender && temp->averageIncome == averageIncome) {
        *head = (*head)->next;
        free(temp);
        return;
    }

    Liver* prev = *head;
    temp = (*head)->next;
    while (temp != NULL) {
        if (strcmp(temp->surname, surname) == 0 && strcmp(temp->name, name) == 0 && strcmp(temp->middleName, middleName) == 0
            && temp->birthDate.day == birthDate.day && temp->birthDate.month == birthDate.month && temp->birthDate.year == birthDate.year
            && temp->gender == gender && temp->averageIncome == averageIncome) {
            prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
}

void printLivers(Liver* head) {
    Liver* curr = head;
    while (curr != NULL) {
        printf("Фамилия: %s\n", curr->surname);
        printf("Имя: %s\n", curr->name);
        printf("Отчество: %s\n", curr->middleName);
        printf("Дата рождения: %d.%d.%d\n", curr->birthDate.day, curr->birthDate.month, curr->birthDate.year);
        printf("Пол: %c\n", curr->gender);
        printf("Средний доход за месяц: %.2lf\n", curr->averageIncome);
        printf("-------------------\n");
        curr = curr->next;
    }
}

void saveDataToFile(Liver* head, const char* filePath) {
    FILE* file = fopen(filePath, "w");
    if (file == NULL) {
        printf("Не удалось создать файл для сохранения данных.\n");
        return;
    }

    Liver* curr = head;
    while (curr != NULL) {
        fprintf(file, "Фамилия: %s\n", curr->surname);
        fprintf(file, "Имя: %s\n", curr->name);
        fprintf(file, "Отчество: %s\n", curr->middleName);
        fprintf(file, "Дата рождения: %d.%d.%d\n", curr->birthDate.day, curr->birthDate.month, curr->birthDate.year);
        fprintf(file, "Пол: %c\n", curr->gender);
        fprintf(file, "Средний доход за месяц: %.2lf\n", curr->averageIncome);
        fprintf(file, "-------------------\n");
        curr = curr->next;
    }

    fclose(file);
}

void freeLivers(Liver* head) {
    Liver* curr = head;
    while (curr != NULL) {
        Liver* temp = curr;
        curr = curr->next;
        free(temp);
    }
}

int main() {
    Liver* livers = NULL;
    int totalModifications = 0;
    int undoLimit = 0;
    int undoCount = 0;

    FILE* file = fopen("livers.txt", "r");
    if (file == NULL) {
        printf("Не удалось открыть файл с информацией о жителях.\n");
        return 1;
    }

    char surname[MAX_NAME_LEN];
    char name[MAX_NAME_LEN];
    char middleName[MAX_NAME_LEN];
    Date birthDate;
    char gender;
    double averageIncome;

    while (fscanf(file, "%s%s%s%d.%d.%d %c%lf", surname, name, middleName, &birthDate.day, &birthDate.month, &birthDate.year, &gender, &averageIncome) == 8) {
        addLiver(&livers, surname, name, middleName, birthDate, gender, averageIncome);
        totalModifications++;
    }

    fclose(file);

    while (1) {
        printf("Выберите действие:\n");
        printf("1. Поиск жителя\n");
        printf("2. Изменение информации о жителе\n");
        printf("3. Удаление информации о жителе\n");
        printf("4. Добавление информации о жителе\n");
        printf("5. Сохранение данных в файл\n");
        printf("6. Отмена последних N/2 модификаций\n");
        printf("0. Выход\n");

        int choice;
        printf("Введите номер действия: ");
        scanf("%d", &choice);

        if (choice == 0) {
            break;
        }

        if (choice == 1) {
            printf("Введите параметры поиска жителя:\n");
            printf("Фамилия: ");
            scanf("%s", surname);
            printf("Имя: ");
            scanf("%s", name);
            printf("Отчество: ");
            scanf("%s", middleName);
            printf("Дата рождения (день месяц год, разделенные пробелом): ");
            scanf("%d %d %d", &birthDate.day, &birthDate.month, &birthDate.year);
            printf("Пол (M/W): ");
            scanf(" %c", &gender);
            printf("Средний доход за месяц: ");
            scanf("%lf", &averageIncome);

            Liver* foundLiver = findLiver(livers, surname, name, middleName, birthDate, gender, averageIncome);
            if (foundLiver != NULL) {
                printf("Найден житель:\n");
                printf("Фамилия: %s\n", foundLiver->surname);
                printf("Имя: %s\n", foundLiver->name);
                printf("Отчество: %s\n", foundLiver->middleName);
                printf("Дата рождения: %d.%d.%d\n", foundLiver->birthDate.day, foundLiver->birthDate.month, foundLiver->birthDate.year);
                printf("Пол: %c\n", foundLiver->gender);
                printf("Средний доход за месяц: %.2lf\n", foundLiver->averageIncome);
            } else {
                printf("Житель не найден.\n");
            }
        } else if (choice == 2) {
            printf("Введите параметры изменения информации о жителе:\n");
            printf("Фамилия: ");
            scanf("%s", surname);
            printf("Имя: ");
            scanf("%s", name);
            printf("Отчество: ");
            scanf("%s", middleName);
            printf("Дата рождения (день месяц год, разделенные пробелом): ");
            scanf("%d %d %d", &birthDate.day, &birthDate.month, &birthDate.year);
            printf("Пол (M/W): ");
            scanf(" %c", &gender);
            printf("Средний доход за месяц: ");
            scanf("%lf", &averageIncome);

            Liver* foundLiver = findLiver(livers, surname, name, middleName, birthDate, gender, averageIncome);
            if (foundLiver != NULL) {
                printf("Исходная информация о жителе:\n");
                printf("Фамилия: %s\n", foundLiver->surname);
                printf("Имя: %s\n", foundLiver->name);
                printf("Отчество: %s\n", foundLiver->middleName);
                printf("Дата рождения: %d.%d.%d\n", foundLiver->birthDate.day, foundLiver->birthDate.month, foundLiver->birthDate.year);
                printf("Пол: %c\n", foundLiver->gender);
                printf("Средний доход за месяц: %.2lf\n", foundLiver->averageIncome);

                char newSurname[MAX_NAME_LEN];
                char newName[MAX_NAME_LEN];
                char newMiddleName[MAX_NAME_LEN];
                Date newBirthDate;
                char newGender;
                double newAverageIncome;

                printf("Введите новую информацию о жителе:\n");
                printf("Фамилия: ");
                scanf("%s", newSurname);
                printf("Имя: ");
                scanf("%s", newName);
                printf("Отч