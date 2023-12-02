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

typedef struct Node {
    char *word;
    int count;
    int wordLength;
    struct Node *left;
    struct Node *right;
} Node;

void freeTree(Node* root) {
    if (root == NULL) {
        return;
    }

    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

Node *createNode(char *word) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }
    node->word = strdup(word);
    node->count = 1;
    node->wordLength = (int)strlen(word);
    node->left = NULL;
    node->right = NULL;
    return node;
}

void insertNode(Node **tree, char *word) {
    if (*tree == NULL) *tree = createNode(word);
    else {
        int cmp = strcmp(word, (*tree)->word);

        if (cmp < 0) insertNode(&(*tree)->left, word);
        else if (cmp > 0) insertNode(&(*tree)->right, word);
        else (*tree)->count++;
    }
}

void printNode(Node *node) {
    if (node != NULL) {
        printNode(node->left);
        printf("%s: %d\n", node->word, node->count);
        printNode(node->right);
    }
}

Node *searchNode(Node *tree, char *word) {
    if (tree == NULL) {
        return NULL;
    }
    int cmp = strcmp(word, tree->word);
    if (cmp < 0) {
        return searchNode(tree->left, word);
    }
    else if (cmp > 0) {
        return searchNode(tree->right, word);
    }
    else {
        return tree;
    }
}

Node *findMinLength(Node *tree, Node *extra) {
    if (extra == NULL)
        extra = tree;


    if(tree!=NULL) {
        if(tree->wordLength < extra->wordLength) extra = tree;

        extra = findMinLength(tree->left, extra);
        extra = findMinLength(tree->right, extra);
    }

    return extra;
}

Node *findMaxLength(Node *tree, Node *extra) {
    if (extra == NULL)
        extra = tree;

    if(tree!=NULL) {
        if(tree->wordLength > extra->wordLength) extra = tree;

        extra = findMaxLength(tree->left, extra);
        extra = findMaxLength(tree->right, extra);
    }

    return extra;
}

Node *findMax(Node *tree, Node *extra, int n, char* word) {
    if (extra == NULL)
        extra = tree;
    if (tree != NULL) {
        if(extra->count < tree->count+1 && (tree->count < n+1 || n == 0) && (strcmp(tree->word, word) != 0 || strcmp("", word) == 0))
            extra = tree;
        extra = findMax(tree->left, extra, n, word);
        extra = findMax(tree->right, extra, n, word);
    }
    return extra;
}

int getDepth(Node *tree) {
    if (tree == NULL) {
        return 0;
    }
    int leftDepth = getDepth(tree->left);
    int rightDepth = getDepth(tree->right);

    if (leftDepth > rightDepth) {
        return leftDepth + 1;
    }
    else {
        return rightDepth + 1;
    }
}

void saveTreeToFile(Node *tree, FILE *file) {
    if (tree != NULL) {
        saveTreeToFile(tree->left, file);
        fwrite(tree->word, sizeof(char), strlen(tree->word), file);
        fwrite("\n", sizeof(char), 1, file);

        char stringInt[90];
        itoa(tree->count, stringInt, 10);

        fwrite(stringInt, sizeof(char), strlen(stringInt), file);
        fwrite("\n", sizeof(char), 1, file);
        saveTreeToFile(tree->right, file);
    }
}

void changeTreeCount(Node **tree, char* word, int count) {
    Node *extra = *tree;
    if(extra != NULL) {
        if(strcmp(extra->word, word) == 0 ) {
            extra->count = count;
            return;
        }

        changeTreeCount(&extra->left, word, count);
        changeTreeCount(&extra->right, word, count);

    }
}

Node *loadTreeFromFile(FILE *file) {
    Node *tree = NULL;
    char word[256];
    int count;

    while (fscanf(file, "%s\n", word) != EOF) {
        fscanf(file, "%d\n", &count);
        insertNode(&tree, word);

        changeTreeCount(&tree, word, count);
    }
    return tree;
}

int main(int argc, char *argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if (argc < 3) {
        printf("%s\n", errorMessages[INCORRECT_COUNT_ARGUMENTS]);
        return INCORRECT_COUNT_ARGUMENTS;
    }

    char *filePath = argv[1];
    FILE *file = fopen(filePath, "r");
    if (file == NULL) {
        printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
        return FILE_OPENING_ERROR;
    }

    Node *tree = NULL;
    char line[256];

    // Read file line by line

    while (fgets(line, sizeof(line), file)) {
        if(ferror(file)) {
            printf("%s\n", errorMessages[FILE_READING_ERROR]);
            free(tree);
            return FILE_READING_ERROR;
        }
        char *word = strtok(line, argv[2]);
        while (word != NULL) {
            // Insert word into the tree
            insertNode(&tree, word);
            word = strtok(NULL, argv[2]);
        }
    }

    // Close the file
    if ( fclose (file) == EOF) {
        printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
        free(tree);
        return FILE_CLOSING_ERROR;
    }

    printf("Введите цифру, соответсвующую одной из опций:\n");
    printf("1. Сколько раз встречается слово 'х' в контексте;\n");
    printf("2. n наиболее часто встречающихся слов в контексте;\n");
    printf("3. Самое длинное слово;\n");
    printf("4. Самое короткое слово;\n");
    printf("5. Найти глубину построенного дерева;\n");
    printf("6. Загрузить дерево в файл;\n");
    printf("7. Выгрузить дерево из файла;\n");
    printf("0. Выйти.\n\n");

    int user_input = -1;
    char numString[90];


    while(user_input != 0) {

        printf("Введите номер опции: ");
        scanf("%s", numString);
        if(strchr("-0123456789", numString[0]) == NULL) {
            printf("Неверный выбор.\n");
            continue;
        }

        user_input = atoi(numString);

        switch (user_input) {
            case 1: {
                char input[256];
                printf("Введите слово для поиска: ");
                scanf("%s", input);
                Node *searchResult = searchNode(tree, input);
                if (searchResult != NULL) printf("Cлово '%s' встречается %d кол-во раз.\n", searchResult->word, searchResult->count);
                else printf("Слово '%s'не найдено.\n", input);
                break;
            }
            case 2: {
                int n;
                printf("Введите n: ");
                scanf("%d", &n);
                printf("Топ %d часто встречающихся слов:\n", n);
                // Print the first n most frequent words
                Node *maxNode = findMax(tree, NULL, 0, "");
                while (n>0) {
                    printf("%s: %d\n", maxNode->word, maxNode->count);
                    maxNode = findMax(tree, NULL, maxNode->count, maxNode->word);
                    n--;
                }
                break;
            }
            case 3: {
                Node *longestWord = findMaxLength(tree, NULL);
                if (longestWord != NULL) {
                    printf("Самое длинное слово: %s\n", longestWord->word);
                }
                break;
            }
            case 4: {
                Node *shortestWord = findMinLength(tree, NULL);
                if (shortestWord != NULL) {
                    printf("Самое короткое слово: %s\n", shortestWord->word);
                }
                break;
            }
            case 5: {
                printf("Глубина дерева: %d\n", getDepth(tree));
                break;
            }
            case 6: {
                FILE *outputFile = fopen("tree.txt", "w");
                if (outputFile == NULL) {
                    printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
                    free(tree);
                    return FILE_OPENING_ERROR;
                }
                saveTreeToFile(tree, outputFile);
                if ( fclose (outputFile) == EOF) {
                    printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
                    free(tree);
                    return FILE_CLOSING_ERROR;
                }
                printf("Дерево загружено!\n");
                break;
            }
            case 7: {
                FILE *inputFile = fopen("tree.txt", "r");
                if (inputFile == NULL) {
                    printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
                    free(tree);
                    return FILE_OPENING_ERROR;
                }
                Node *loadedTree = loadTreeFromFile(inputFile);
                if ( fclose (inputFile) == EOF) {
                    printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
                    free(tree);
                    return FILE_CLOSING_ERROR;
                }

                // Print the word count for each word in the loaded tree
                printf("Загруженное дерево:\n");
                printNode(loadedTree);
                break;
            }
            case 0:
                break;
            default:
                printf("Неверный выбор!\n");
                break;
        }
    }
    // Print the word count for each word in the tree
//    printf("Word count:\n");
//    printNode(tree);

    // Prompt user for word input
//    char input[256];
//    printf("Enter a word to search: ");
//    scanf("%s", input);
//    Node *searchResult = searchNode(tree, input);
//    if (searchResult != NULL) printf("'%s' found %d times in the file.\n", searchResult->word, searchResult->count);
//    else printf("'%s' not found in the file.\n", input);

//  Prompt user for n value
//    int n;
//    printf("Введите n: ");
//    scanf("%d", &n);
//    printf("Топ %d часто встречающихся слов:\n", n);
//    // Print the first n most frequent words
//    Node *maxNode = findMax(tree, NULL, 0, "");
//    while (n>0) {
//        printf("%s: %d\n", maxNode->word, maxNode->count);
//        maxNode = findMax(tree, NULL, maxNode->count, maxNode->word);
//        n--;
//    }

    // Find the longest word
//    Node *longestWord = findMaxLength(tree, NULL);
//    if (longestWord != NULL) {
//        printf("Самое длинное слово: %s\n", longestWord->word);
//    }

    // Find the shortest word
//    Node *shortestWord = findMinLength(tree, NULL);
//    if (shortestWord != NULL) {
//        printf("Самое короткое слово: %s\n", shortestWord->word);
//    }

//    printf("Глубина дерева: %d\n", getDepth(tree));

    // Save the tree to a file
//    FILE *outputFile = fopen("tree.txt", "w");
//    if (outputFile == NULL) {
//        printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
//        return FILE_OPENING_ERROR;
//    }
//    saveTreeToFile(tree, outputFile);
//    fclose(outputFile);

    // Load the tree from the file
//    FILE *inputFile = fopen("tree.txt", "r");
//    if (inputFile == NULL) {
//        printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
//        return FILE_OPENING_ERROR;
//    }
//    Node *loadedTree = loadTreeFromFile(inputFile);
//    fclose(inputFile);
//
//    // Print the word count for each word in the loaded tree
//    printf("Загруженное дерево:\n");
//    printNode(loadedTree);

    freeTree(tree);
    return SUCCESS;
}
