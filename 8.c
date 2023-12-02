#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <Windows.h>

typedef enum {
    SUCCESS = 0,
    INCORRECT_FILE_INPUT,
    OVERFLOW_ERROR,
    MEMORY_ALLOCATION_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    FILE_CLOSING_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Некорректное содержимое в файле, попробуйте ещё раз",
        "Произошло переполнение",
        "Проблемы с выделением памяти",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Ошибка закрытия файла",
        "Неизвестная ошибка, что-то пошло не так"
};

// Структура, представляющая одночлен многочлена
typedef struct term {
    int coeff;          // коэффициент
    int power;          // степень
    struct term* next;  // указатель на следующий одночлен
} Term;

// Функция для создания нового одночлена
Term* createTerm(int coeff, int power) {
    Term* newTerm = (Term*)malloc(sizeof(Term));

    if (newTerm == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }

    newTerm->coeff = coeff;
    newTerm->power = power;
    newTerm->next = NULL;
    return newTerm;
}

// Функция для вставки одночлена в конец многочлена
void insertTerm(Term** poly, int coeff, int power) {
    Term* newTerm = createTerm(coeff, power);

    if (*poly == NULL) *poly = newTerm;
    else {
        Term* curr = *poly;
        while (curr->next != NULL) curr = curr->next;
        curr->next = newTerm;
    }
}

// Функция для сложения двух многочленов
Term* addPolynomials(Term* poly1, Term* poly2) {
    Term* sum = NULL;
    while (poly1 != NULL && poly2 != NULL) {
        if (poly1->power > poly2->power) {
            insertTerm(&sum, poly1->coeff, poly1->power);
            poly1 = poly1->next;
        } else if (poly1->power < poly2->power) {
            insertTerm(&sum, poly2->coeff, poly2->power);
            poly2 = poly2->next;
        } else {
            int coeffSum = poly1->coeff + poly2->coeff;
            if (coeffSum != 0) {
                insertTerm(&sum, coeffSum, poly1->power);
            }
            poly1 = poly1->next;
            poly2 = poly2->next;
        }
    }

    // Добавляем оставшиеся одночлены, если есть
    while (poly1 != NULL) {
        insertTerm(&sum, poly1->coeff, poly1->power);
        poly1 = poly1->next;
    }
    while (poly2 != NULL) {
        insertTerm(&sum, poly2->coeff, poly2->power);
        poly2 = poly2->next;
    }

    return sum;
}

// Функция для вычитания многочлена из многочлена
Term* subtractPolynomials(Term* poly1, Term* poly2) {
    Term* diff = NULL;
    while (poly1 != NULL && poly2 != NULL) {
        if (poly1->power > poly2->power) {
            insertTerm(&diff, poly1->coeff, poly1->power);
            poly1 = poly1->next;
        } else if (poly1->power < poly2->power) {
            insertTerm(&diff, -poly2->coeff, poly2->power);
            poly2 = poly2->next;
        } else {
            int coeffDiff = poly1->coeff - poly2->coeff;
            if (coeffDiff != 0) insertTerm(&diff, coeffDiff, poly1->power);
            poly1 = poly1->next;
            poly2 = poly2->next;
        }
    }

    // Добавляем оставшиеся одночлены, если есть
    while (poly1 != NULL) {
        insertTerm(&diff, poly1->coeff, poly1->power);
        poly1 = poly1->next;
    }
    while (poly2 != NULL) {
        insertTerm(&diff, -poly2->coeff, poly2->power);
        poly2 = poly2->next;
    }

    return diff;
}

// Функция для умножения многочленов
Term* multiplyPolynomials(Term* poly1, Term* poly2) {
    Term* product = NULL;
    Term* temp = NULL;

    while (poly1 != NULL) {
        temp = poly2;
        while (temp != NULL) {
            int coeffProduct = poly1->coeff * temp->coeff;
            int powerProduct = poly1->power + temp->coeff;
            insertTerm(&product, coeffProduct, powerProduct);
            temp = temp->next;
        }
        poly1 = poly1->next;
    }

    free(temp);
    return product;
}

Term* multiplyPolynomialsForDiv(Term* poly1, int poly2Coeff, int poly2Power) {
    Term* product = NULL;

    while (poly1 != NULL) {
        int coeffProduct = poly1->coeff * poly2Coeff;
        int powerProduct = poly1->power + poly2Power;
        insertTerm(&product, coeffProduct, powerProduct);
        poly1 = poly1->next;
    }

    return product;
}

// Функция для деления многочлена на многочлен нацело
Term* dividePolynomials(Term* dividend, Term* divisor, Term** remainder) {
    Term* quotient = NULL;
    *remainder = NULL;
    while (dividend != NULL && dividend->power >= divisor->power) {
        int coeffQuotient = dividend->coeff / divisor->coeff;
        int powerQuotient = dividend->power - divisor->power;
        insertTerm(&quotient, coeffQuotient, powerQuotient);

        Term* temp = multiplyPolynomialsForDiv(divisor, coeffQuotient, powerQuotient);
        Term* sub = subtractPolynomials(dividend, temp);

        free(temp);
        free(*remainder);
        *remainder = sub;

        dividend = sub;
//        free(sub);
    }

    return quotient;
}

// Функция для вычисления значения многочлена в заданной точке
int evaluatePolynomial(Term* poly, int x) {
    int result = 0;
    while (poly != NULL) {
        result += poly->coeff * (int)pow(x, poly->power);
        poly = poly->next;
    }
    return result;
}

// Функция для нахождения производной многочлена
Term* differentiatePolynomial(Term* poly) {
    Term* derivative = NULL;
    while (poly != NULL) {
        if (poly->power > 0) {
            int coeffDerivative = poly->coeff * poly->power;
            int powerDerivative = poly->power - 1;
            insertTerm(&derivative, coeffDerivative, powerDerivative);
        }
        poly = poly->next;
    }
    return derivative;
}

// Функция для композиции двух многочленов
Term* composePolynomials(Term* poly1, Term* poly2) {
    Term* composition = NULL;
    while (poly1 != NULL) {
        Term* temp = poly2;
        Term* result = NULL;

        while (temp != NULL) {
            int coeff = poly1->coeff * temp->coeff;
            int power = poly1->power + temp->power;
            insertTerm(&result, coeff, power);
            temp = temp->next;
        }

        if (composition == NULL) composition = result;
        else {
            composition = addPolynomials(composition, result);
            free(result);
        }

        poly1 = poly1->next;
        free(temp);
    }
    return composition;
}

// Функция для освобождения памяти, занятой многочленом
void freePolynomial(Term *poly) {
    while (poly != NULL) {
        Term* temp = poly;
        poly = poly->next;
        free(temp);
    }
}

// Функция для чтения многочлена из строки
Term* readPolynomial(char* expression) {
    Term* poly = NULL;
    int coeff = 0;
    int power = 0;
    int flag = 0;
    int sign;
    int length = 0;
    char signs[strlen(expression)];


    for(int i=0; i < strlen(expression); i++)
        if(strchr("+-", expression[i])) signs[i] = expression[i];
        else signs[i] = '+';

    if(signs[0] != '-')
        length--;

    char* ptr = strtok(expression, "+-");

    while (ptr != NULL) {
        if(signs[length] == '-') sign = -1;
        else sign = 1;

        int len = strlen(ptr); length += len+1;
        int i;

        for (i = 0; i < len; i++) {
            if (ptr[i] == 'x' || ptr[i] == 'X') {
                flag = 1;
                break;
            }
        }

        if (flag == 1) {
            int j;
            char* temp = (char*)malloc(len * sizeof(char));
            if (temp == NULL) {
                printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
                exit(MEMORY_ALLOCATION_ERROR);
            }
            for (j = 0; j < i; j++) temp[j] = ptr[j];
            temp[j] = '\0';
            coeff = atoi(temp);
            if(i==0 && coeff==0) coeff=1;
            coeff = sign * coeff;
            free(temp);

            power = 1;
            if (i < len - 1 && (ptr[i + 1] == '^' || ptr[i + 1] == '-')) {
                // Пропускаем знак степени или символ '^'
                i += 2;

                temp = (char*)malloc((len - i) * sizeof(char));
                if (temp == NULL) {
                    printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
                    exit(MEMORY_ALLOCATION_ERROR);
                }
                int k = 0;
                while (ptr[i] != '\0') {
                    temp[k] = ptr[i];
                    i++; k++;
                }
                temp[k] = '\0';
                power = atoi(temp);
                free(temp);
            }

            insertTerm(&poly, coeff, power);
            flag = 0;
        } else {
            coeff = atoi(ptr);
            coeff *= sign;
            insertTerm(&poly, coeff, 0);
        }

        ptr = strtok(NULL, "+-");
    }
    return poly;
}

// Функция для печати многочлена
void printPolynomial(Term* poly) {
    if (poly == NULL) {
        printf("0");
        return;
    }

    while (poly != NULL) {
        if (poly->coeff != 0) {
            if (poly->coeff > 0) printf("+");

            if (poly->power == 0) printf("%d", poly->coeff);
            else if (poly->power == 1) {
                if (poly->coeff != 1) printf("%dx", poly->coeff);
                else printf("x");
            } else {
                if (poly->coeff != 1) printf("%dx^%d", poly->coeff, poly->power);
                else printf("x^%d", poly->power);
            }
        }
        poly = poly->next;
    }
}

// Функция для обработки инструкций
void processInstructions(FILE* file) {
    int accumulator, isPolylineComm = 0, isEndComm = 0;
    char line[100];
    Term* result = NULL;

    while (fgets(line, sizeof(line), file)) {
        if (ferror(file)) exit(FILE_READING_ERROR);
        if (line[0] == '%' || strlen(line) == 1 && (line[0] == ' ' || line[0] == '\n')) continue;
        else if (line[0] == '[') {
            while (fgets(line, sizeof(line), file)) {
                if (ferror(file)) exit(FILE_READING_ERROR);
                isEndComm = 0;

                for(int i=0; i < strlen(line)-1; i++) {
                    if(strchr("]", line[0]) != NULL) {
                        isEndComm = 1;
                        break;
                    }
                }

                if(isEndComm) break;

            }
        }

        while (line[0] == 32 || strchr("]", line[0]) != NULL && isEndComm == 1 || strchr("[", line[0]) != NULL && strchr(line, ']') != NULL) {
            if(strchr("]", line[0]) != NULL) isEndComm = 0;
            for(int i=0; i < strlen(line)-1; i++) {
                if(strchr("[", line[0]) != NULL) isEndComm = 1;
                line[i] = line[i+1];
            }
            line[strlen(line)-1] = '\0';
        }



        char* instruction = strtok(line, "(");
        char* expression = strtok(NULL, ",)");
        char* arg = strtok(NULL, ",);");
        char* extraStr = strtok(NULL, ");");
        Term* poly1;
        Term* poly2;

        Term* remainder;
        int value;

        if (strcmp(instruction, "Add") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0 || strcmp("\n", arg) == 0) poly2 = result;
            else poly2 = readPolynomial(arg);
            poly1 = readPolynomial(expression);

            result = addPolynomials(poly1, poly2);
        } else if (strcmp(instruction, "Sub") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0 || strcmp("\n", arg) == 0) {
                poly1 = result;
                poly2 = readPolynomial(expression);
            } else {
                poly2 = readPolynomial(arg);
                poly1 = readPolynomial(expression);
            }

            result = subtractPolynomials(poly1, poly2);
        } else if (strcmp(instruction, "Mult") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0 || strcmp("\n", arg) == 0) poly2 = result;
            else poly2 = readPolynomial(arg);
            poly1 = readPolynomial(expression);

            result = multiplyPolynomials(poly1, poly2);
        } else if (strcmp(instruction, "Div") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0) {
                poly1 = result;
                poly2 = readPolynomial(expression);
            } else {
                poly2 = readPolynomial(arg);
                poly1 = readPolynomial(expression);
            }

            result = dividePolynomials(poly1, poly2, &remainder);
            freePolynomial(remainder);
        } else if (strcmp(instruction, "Mod") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0 || strcmp("\n", arg) == 0) {
                poly1 = result;
                poly2 = readPolynomial(expression);
            } else {
                poly2 = readPolynomial(arg);
                poly1 = readPolynomial(expression);
            }

            result = dividePolynomials(poly1, poly2, &remainder);
            freePolynomial(result);
            result = remainder;
        } else if (strcmp(instruction, "Eval") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0 || strcmp("\n", arg) == 0){
                poly1 = result;
                value = atoi(expression);
            } else {
                poly1 = readPolynomial(expression);
                value = atoi(arg);
            }
            accumulator = evaluatePolynomial(poly1, value);
            printf("Результат: %d\n", accumulator);
            freePolynomial(poly1);
        } else if (strcmp(instruction, "Diff") == 0) {
            poly1 = readPolynomial(expression);
            result = differentiatePolynomial(poly1);
            freePolynomial(poly1);
        } else if (strcmp(instruction, "Cmps") == 0) {
            if(arg == NULL || strcmp(" ", arg) == 0 || strcmp("\n", arg) == 0) {
                poly1 = result;
                poly2 = readPolynomial(expression);
            } else {
                poly2 = readPolynomial(arg);
                poly1 = readPolynomial(expression);
            }

            result = composePolynomials(poly1, poly2);
            freePolynomial(poly1);
            freePolynomial(poly2);
        } else if (extraStr == NULL) {
            if (strcmp(instruction, "Add") != 0 || strcmp(instruction, "Sub") != 0 || strcmp(instruction, "Mult") != 0
                || strcmp(instruction, "Div") != 0 || strcmp(instruction, "Mod") != 0
                || strcmp(instruction, "Diff") != 0 || strcmp(instruction, "Cmps") != 0 || strcmp(instruction, "Eval") != 0) {
                freePolynomial(result);
                printf("%s\n", errorMessages[INCORRECT_FILE_INPUT]);
                exit(INCORRECT_FILE_INPUT);
            }
        } else if (strcmp(" ", extraStr) != 0 || strcmp("\n", extraStr) != 0 ) {
            if (strcmp(instruction, "Add") != 0 || strcmp(instruction, "Sub") != 0 || strcmp(instruction, "Mult") != 0
                || strcmp(instruction, "Div") != 0 || strcmp(instruction, "Mod") != 0
                || strcmp(instruction, "Diff") != 0 || strcmp(instruction, "Cmps") != 0 || strcmp(instruction, "Eval") != 0) {
                freePolynomial(result);
                printf("%s\n", errorMessages[INCORRECT_FILE_INPUT]);
                exit(INCORRECT_FILE_INPUT);
            }
        } else {
            freePolynomial(result);
            printf("%s\n", errorMessages[INCORRECT_FILE_INPUT]);
            exit(INCORRECT_FILE_INPUT);
        }

        if(extraStr != NULL) {
            for(int i=0; i<strlen(extraStr); i++) {
                if (strchr("%", extraStr[i]) != NULL) {
                    break;
                } else if (strchr("[", extraStr[i]) != NULL && strchr("]", extraStr[i]) == NULL) {
                    isPolylineComm = 1;
                    isEndComm = 1;
                    break;
                } else if (strchr("[", extraStr[i]) != NULL && strchr("]", extraStr[i]) != NULL) {
                    for(int j=i+1; j<strlen(extraStr); j++){
                        if(strchr("]", extraStr[j]) != NULL) break;
                        i++;
                    }
                } else if (extraStr[i] != 10 && extraStr[i] != 32) {
                    freePolynomial(result);
                    printf("%s\n", errorMessages[INCORRECT_FILE_INPUT]);
                    exit(INCORRECT_FILE_INPUT);
                }
            }
        }

        if(isPolylineComm) {
            isPolylineComm = 0;
            continue;
        }

        if (strcmp(instruction, "Add") == 0 || strcmp(instruction, "Sub") == 0 || strcmp(instruction, "Mult") == 0
            || strcmp(instruction, "Div") == 0 || strcmp(instruction, "Mod") == 0
            || strcmp(instruction, "Diff") == 0 || strcmp(instruction, "Cmps") == 0) {
            printf("Результат: ");
            printPolynomial(result);
            printf("\n");
        }
    }
    freePolynomial(result);
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    FILE* file = fopen("instructions.txt", "r");
    if (file == NULL) {
        printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
        return FILE_OPENING_ERROR;
    }

    processInstructions(file);

    if (fclose (file) == EOF) {
        printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
        return FILE_CLOSING_ERROR;
    }

    return SUCCESS;
}
