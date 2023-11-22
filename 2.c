#include <stdio.h>
#include <stdarg.h>
#include <math.h>
#include <Windows.h>

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
        "Всё хорошо",
        "Некорректный ввод, попробуйте ещё раз",
        "Произошло переполнение",
        "Проблемы с выделением памяти",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Неизвестная ошибка, что-то пошло не такф"
};

struct Vector {
    double* coordinates;
};

double norm_1(int n, struct Vector* vectors, ...) {
    double max_norm = 0.0;
    va_list args;
    va_start(args, vectors);

    for (int i = 0; i < n; ++i) {
        struct Vector vector = vectors[i];
        double norm = 0.0;

        for (int j = 0; j < n; ++j) {
            norm += fabs(vector.coordinates[j]);
        }

        if (norm > max_norm)
            max_norm = norm;
    }

    va_end(args);
    return max_norm;
}

double norm_p(int n, struct Vector* vectors, double p, ...) {
    double max_norm = 0.0;
    va_list args;
    va_start(args, p);

    for (int i = 0; i < n; ++i) {
        struct Vector vector = vectors[i];
        double norm = 0.0;

        for (int j = 0; j < n; ++j) {
            norm += pow(fabs(vector.coordinates[j]), p);
        }

        norm = pow(norm, 1.0 / p);

        if (norm > max_norm) {
            max_norm = norm;
        }
    }

    va_end(args);
    return max_norm;
}

double norm_A(int n, struct Vector* vectors, double (*matrix)(int n, struct Vector, struct Vector), ...) {
    double max_norm = 0.0;
    va_list args;
    va_start(args, matrix);

    for (int i = 0; i < n; ++i) {
        struct Vector vector = vectors[i];
        double norm = matrix(n, vector, vector);

        if (norm > max_norm) {
            max_norm = norm;
        }
    }

    va_end(args);
    return max_norm;
}

double dotProduct(int n, struct Vector vector1, struct Vector vector2) {
    double product = 0.0;

    for (int i = 0; i < n; ++i) {
        product += vector1.coordinates[i] * vector2.coordinates[i];
    }

    return product;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    struct Vector vectors[3];

    double coordinates1[3] = {1.0, 2.0, 3.0};
    vectors[0].coordinates = coordinates1;

    double coordinates2[3] = {4.0, 5.0, 6.0};
    vectors[1].coordinates = coordinates2;

    double coordinates3[3] = {7.0, 8.0, 9.0};
    vectors[2].coordinates = coordinates3;

    double max_norm_1 = norm_1(3, vectors);
    printf("Максимальная норма по ||x||inf: %.2f\n", max_norm_1);

    double max_norm_p = norm_p(3, vectors, 2.0);
    printf("Максимальная норма по ||x||p: %.2f\n", max_norm_p);

    double max_norm_A = norm_A(3, vectors, dotProduct);
    printf("Максимальная норма по ||x||A: %.2f\n", max_norm_A);

    return SUCCESS;
}
