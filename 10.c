#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EXPRESSION_LEN 100

typedef struct Node {
    char data;
    struct Node* left;
    struct Node* right;
} Node;

Node* createNode(char data) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

void insertNode(Node** root, char data) {
    if (*root == NULL) {
        *root = createNode(data);
    } else {
        if (data == '(' || data == ')') {
            if ((*root)->left == NULL) {
                insertNode(&((*root)->left), data);
            } else if ((*root)->right == NULL) {
                insertNode(&((*root)->right), data);
            } else {
                insertNode(&((*root)->left), data);
            }
        }
    }
}

void printTree(Node* root, int space) {
    if (root == NULL) {
        return;
    }
    space += 5;

    printTree(root->right, space);
    printf("\n");
    for (int i = 5; i < space; i++) {
        printf(" ");
    }
    printf("%c\n", root->data);

    printTree(root->left, space);
}

void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

void processExpressions(const char* inputFile, const char* outputFile) {
    FILE* input = fopen(inputFile, "r");
    if (input == NULL) {
        printf("Не удалось открыть входной файл.\n");
        return;
    }

    FILE* output = fopen(outputFile, "w");
    if (output == NULL) {
        printf("Не удалось создать выходной файл.\n");
        fclose(input);
        return;
    }

    char expression[MAX_EXPRESSION_LEN];
    while (fgets(expression, MAX_EXPRESSION_LEN, input) != NULL) {
        int len = strlen(expression);
        if (expression[len - 1] == '\n') {
            expression[len - 1] = '\0';
        }

        Node* root = NULL;
        for (int i = 0; i < len; i++) {
            insertNode(&root, expression[i]);
        }

        printTree(root, 0);
        fprintf(output, "----------------\n");
        freeTree(root);
    }

    fclose(input);
    fclose(output);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Необходимо указать путь к входному файлу и путь к выходному файлу.\n");
        return 1;
    }

    processExpressions(argv[1], argv[2]);

    return 0;
}
