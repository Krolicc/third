#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define printf(format, ...) printf(format, ##__VA_ARGS__); fflush(stdout);

typedef enum {
    SUCCESS = 0,
    INCORRECT_INPUT,
    OVERFLOW_ERROR,
    MALLOC_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
    "Всё хорошо, можно идти пить чай ☕",
    "Некорректный ввод, попробуйте ещё раз 🤨",
    "Произошло переполнение, ой 🤯",
    "Проблемы с выделением памяти, грустно 😐",
    "Не удалось открыть файл, грустно 😥",
    "Файл прочитан не полностью, грустно 😿",
    "Неизвестная ошибка, что-то пошло не так 🫢"
};

typedef struct String
{
    char *data;
    int len;
} String;

ErrorCode createString(String *str, char *input) {
    if (str == NULL || input == NULL)
        return INCORRECT_INPUT;
    
    char *i = input;
    int cnt = 0;
    while (i != NULL) {
        if (*i == '\0')
            break;
        ++cnt;
        ++i;
    }
    if (i == NULL)
        return INCORRECT_INPUT;
    
    char *memoryData = (char *)malloc((cnt + 1) * sizeof(char));
    if (memoryData == NULL) 
        return MALLOC_ERROR;

    i = input;
    int j = 0;
    while (*i != '\0') {
        memoryData[j] = *i;
        ++i;
        ++j;
    }
    memoryData[j] = '\0';
    str->data = memoryData;
    str->len = cnt;

    return SUCCESS;
}

ErrorCode deleteString(String *str) {
    if (str == NULL)
        return INCORRECT_INPUT;
    str->len = 0;
    free(str->data);
    str->data = NULL;
    return SUCCESS;
}

ErrorCode compareSting(const String *str1, const String *str2, int *answ) {
    if (str1 == NULL || str2 == NULL)
        return INCORRECT_INPUT;
    if (str1->data == NULL || str2->data == NULL)
        return INCORRECT_INPUT;
    
    if (str1->len > str2->len) {
        *answ = 1;
        return SUCCESS;
    } else if (str1->len < str2->len) {
        *answ = 2;
        return SUCCESS;
    }

    for (int i = 0; i < str1->len; ++i) {
        if (str1->data[i] < str2->data[i]) {
            *answ = 1;
            return SUCCESS;
        } else if (str1->data[i] > str2->data[i]) {
            *answ = 2;
            return SUCCESS;
        }
    }

    *answ = 0;
    return SUCCESS;
}

ErrorCode equalString(const String *str1, const String *str2, int *answ) {
    ErrorCode code;
    int bigger;
    code = compareSting(str1, str2, &bigger);
    if (code != SUCCESS) {
        return code;
    }

    if (bigger == 0) {
        *answ = 1;
    } else {
        *answ = 0;
    }
    return SUCCESS;
}

ErrorCode copyString(const String *from, String *to) {
    ErrorCode code;
    code = createString(to, from->data);
    return code;
}

ErrorCode concatString(String *start, const String *end) {
    String temp;
    switch (copyString(start, &temp)) {
        default:
            break;
        case INCORRECT_INPUT:
            return INCORRECT_INPUT;
        case MALLOC_ERROR:
            return MALLOC_ERROR;
    }
    
    //start->data = (char *) realloc(start->data, (start->len + end->len + 1) * sizeof(char));
    start->data = (char *) realloc(start->data, (1) * sizeof(char));
    if (start->data == NULL)
        return MALLOC_ERROR;

    int i;
    for (i = 0; i < temp.len - 1; ++i) {
        start->data[i] = temp.data[i];
    }
    --i;
    for (int j = 0; j < end->len; ++j) {
        ++i;
        start->data[i] = end->data[j];
    }
    start->data[i + 1] = '\0';
    start->len = start->len + end->len;
    
    switch (deleteString(&temp)) {
        default:
            break;
        case INCORRECT_INPUT:
            return INCORRECT_INPUT;
    }
    return SUCCESS;
}

int main() {

    char input1[] = "123";
    char input2[] = "456";
    //char *input2 = "hello f g";
    String str1;
    String str2;

    createString(&str1, input1);
    createString(&str2, input2);
    int bigger;
    compareSting(&str1, &str2, &bigger);
    printf("%d\n", bigger);

    //copyString(&str1, &str2);
    concatString(&str1, &str2);


    deleteString(&str1);
    deleteString(&str2);
    return SUCCESS;
}