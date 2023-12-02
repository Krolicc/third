#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>

typedef enum {
    SUCCESS = 0,
    INCORRECT_INPUT_ARGUMENTS,
    OVERFLOW_ERROR,
    MEMORY_ALLOCATION_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    FILE_WRITING_ERROR,
    FILE_CLOSING_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Некорректный ввод аргументов, попробуйте ещё раз",
        "Произошло переполнение",
        "Проблемы с выделением памяти",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Ошибка записи в файл",
        "Не удалось закрыть файл",
        "Неизвестная ошибка, что-то пошло не так"
};

#define MAX_EXPRESSION_LEN 100

typedef struct Node {
    char data;
    struct Node* child;
    struct Node* neighbor;
    struct Node* parent;
} Node;

Node* createNode(char data, Node** parent) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }
    newNode->data = data;
    newNode->child = NULL;
    newNode->neighbor = NULL;
    newNode->parent = *parent;
    return newNode;
}

Node* createRoot(char data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }
    newNode->data = data;
    newNode->child = NULL;
    newNode->neighbor = NULL;
    newNode->parent = NULL;
    return newNode;
}

void insertNode(Node** root, char* expression) {
    Node* extra = NULL;
    Node* extraNode = NULL;

    for(int i = 0; i < strlen(expression); i++) {
        if (*root == NULL) {
            *root = createRoot(expression[0]);
        } else {
            if (strchr("(", expression[i]) != NULL) {
                i++;
                while(strchr(" ", expression[i]) != NULL) i++;

                extra = *root;
                *root = (*root)->child;
                *root = createNode(expression[i], &extra);
                extra->child = *root;

//                printf("1. %c %c\n", extra->data, (*root)->data);

            } else if (strchr(",", expression[i]) != NULL) {
                i++;
                while(strchr(" ", expression[i]) != NULL) i++;

                extra = (*root)->parent;
                extraNode = *root;
                *root = (*root)->neighbor;
                *root = createNode(expression[i], &extra);
                extraNode->neighbor = *root;

//                printf("2. %c %c\n", extra->data, (*root)->data);

            } else if (strchr(")", expression[i]) != NULL) {
                *root = (*root)->parent;

//                printf("3. %c\n", (*root)->data);
            }
        }
    }
}

void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }

    freeTree(root->neighbor);
    freeTree(root->child);
    free(root);
}

void printTree(FILE *output, Node* root, int space) {
    if (root == NULL) return;

    space += 5;

    printTree(output, root->child, space);
    if(fprintf(output, "\n") < 0) {
        printf("%s", errorMessages[FILE_WRITING_ERROR]);
        freeTree(root);
        exit(FILE_WRITING_ERROR);
    }

    for (int i = 5; i < space; i++)
       if( fprintf(output, " ") < 0) {
           printf("%s", errorMessages[FILE_WRITING_ERROR]);
           freeTree(root);
           exit(FILE_WRITING_ERROR);
       }

    if (fprintf(output, "%c\n", root->data) < 0 ){
        printf("%s", errorMessages[FILE_WRITING_ERROR]);
        freeTree(root);
        exit(FILE_WRITING_ERROR);
    }

    printTree(output, root->neighbor, space);
}

void processExpressions(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "r");

    if (input == NULL) {
        printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
        return;
    }

    FILE* output = fopen(outputFile, "w");

    if (output == NULL) {
        printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
        fclose(input);
        return;
    }

    char expression[MAX_EXPRESSION_LEN];
    while (fgets(expression, MAX_EXPRESSION_LEN, input) != NULL) {
        if(ferror(input)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
            exit(FILE_READING_ERROR);
        }

        int len = strlen(expression);
        if (expression[len - 1] == '\n') {
            expression[len - 1] = '\0';
        }

        Node* root = NULL;
        insertNode(&root, expression);
        printTree(output, root, 0);

        if(fprintf(output, "-------------------------------------------\n") < 0) {
            printf("%s", errorMessages[FILE_WRITING_ERROR]);
            freeTree(root);
            exit(FILE_WRITING_ERROR);
        }
        freeTree(root);
    }

    if ( fclose (input) == EOF) {
        printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
        exit(FILE_CLOSING_ERROR);
    }
    if ( fclose (output) == EOF) {
        printf("%s", errorMessages[FILE_CLOSING_ERROR]);
        exit(FILE_CLOSING_ERROR);
    }
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);


    if (argc < 3) {
        printf("%s", errorMessages[INCORRECT_INPUT_ARGUMENTS]);
        return INCORRECT_INPUT_ARGUMENTS;
    }

    processExpressions(argv[1], argv[2]);

    return SUCCESS;
}
