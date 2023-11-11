#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
        "Неизвестная ошибка, что-то пошло не так"
};

typedef struct Node {
    char* word;
    int count;
    struct Node* left;
    struct Node* right;
} Node;

void saveTreeToFile(FILE* file, Node* root) {
    if (root != NULL) {
        saveTreeToFile(file, root->left);
        fwrite(root, sizeof(Node), 1, file);
        saveTreeToFile(file, root->right);
    }
}

int getDepth(Node* root) {
    if (root == NULL)
        return 0;

    int leftDepth = getDepth(root->left);
    int rightDepth = getDepth(root->right);

    if (leftDepth > rightDepth)
        return leftDepth + 1;
    else
        return rightDepth + 1;
}

Node* restoreTreeFromFile(FILE* file) {
    Node* node = (Node*)malloc(sizeof(Node));
    fread(node, sizeof(Node), 1, file);

    if (node == NULL)
        return NULL;

    node->word = strdup(node->word);
    node->left = restoreTreeFromFile(file);
    node->right = restoreTreeFromFile(file);

    return node;
}

Node* createNode(char* word) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->word = strdup(word);
    newNode->count = 1;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

Node* insertNode(Node* root, char* word) {
    if (root == NULL)
        return createNode(word);

    int cmp = strcmp(word, root->word);
    if (cmp == 0)
        root->count++;
    else if (cmp < 0)
        root->left = insertNode(root->left, word);
    else
        root->right = insertNode(root->right, word);

    return root;
}

void displayWordCount(Node* root) {
    if (root != NULL) {
        displayWordCount(root->left);
        printf("%s: %d\n", root->word, root->count);
        displayWordCount(root->right);
    }
}

Node* findMin(Node* root) {
    if (root == NULL)
        return NULL;
    else if (root->left == NULL)
        return root;
    else
        return findMin(root->left);
}

Node* findMax(Node* root) {
    if (root == NULL)
        return NULL;
    else if (root->right == NULL)
        return root;
    else
        return findMax(root->right);
}

Node* searchWord(Node* root, char* word) {
    if (root == NULL || strcmp(root->word, word) == 0)
        return root;
    else if (strcmp(word, root->word) < 0)
        return searchWord(root->left, word);
    else
        return searchWord(root->right, word);
}

void displayMostFrequent(Node* root, int n) {
    if (root == NULL)
        return;

    displayMostFrequent(root->right, n);

    if (n > 0) {
        printf("%s: %d\n", root->word, root->count);
        n--;
    }

    displayMostFrequent(root->left, n);
}

char* longestWord(Node* root) {
    if (root == NULL)
        return NULL;

    char* longest = root->word;
    int longestLength = strlen(longest);

    Node* current = root;
    while (current != NULL) {
        if (strlen(current->word) > longestLength) {
            longest = current->word;
            longestLength = strlen(longest);
        }
        current = current->left;
    }

    return longest;
}

char* shortestWord(Node* root) {
    if (root == NULL)
        return NULL;

    char* shortest = root->word;
    int shortestLength = strlen(shortest);

    Node* current = root;
    while (current != NULL) {
        if (strlen(current->word) < shortestLength) {
            shortest = current->word;
            shortestLength = strlen(shortest);
        }
        current = current->left;
    }

    return shortest;
}

//void saveTreeToFile(FILE* file, Node* root) {
//    if (root != NULL) {
//        saveTreeToFile(file, root->left);
//        fwrite(root, sizeof(Node), 1, file);
//        saveTreeToFile(file, root->right);
//    }
//}
//
//Node* restoreTreeFromFile(FILE* file) {
//    Node* node = (Node*)malloc(sizeof(Node));
//    fread(node, sizeof(Node), 1, file);
//
//    if (node == NULL)
//        return NULL;
//
//    node->left = restoreTreeFromFile(file);
//    node->right = restoreTreeFromFile(file);
//
//    return node;
//}

void freeTree(Node* root) {
    if (root == NULL)
        return;

    free(root->word);
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: ./word_stats <file_path> <separator1> <separator2> ...\n");
        return 1;
    }

    char* file_path = argv[1];
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        printf("Failed to open file: %s\n", file_path);
        return 1;
    }

    Node* root = NULL;
    char word[100];

    while (fscanf(file, "%99[^", " \t\n]%*c", word) == 1) {
        for (int i = 2; i < argc; i++) {
            char* separator = argv[i];
            char* token = strtok(word, separator);
            while (token != NULL) {
                root = insertNode(root, token);
                token = strtok(NULL, separator);
            }
        }
    }

    fclose(file);

    int option;
    while (1) {
        printf("\nOptions:\n");
        printf("1. Display word count\n");
        printf("2. Search word count\n");
        printf("3. Display most frequent words\n");
        printf("4. Display longest and shortest words\n");
        printf("5. Save tree to file\n");
        printf("6. Restore tree from file\n");
        printf("7. Exit\n");
        printf("Enter option number: ");
        scanf("%d", &option);
        getchar();

        switch (option) {
            case 1:
                displayWordCount(root);
                break;
            case 2: {
                char searchWord[100];
                printf("Enter word to search: ");
                scanf("%99[^\n]", searchWord);
                getchar();

                Node* searchResult = searchWord(root, searchWord);
                if (searchResult != NULL)
                    printf("Word '%s' count: %d\n", searchWord, searchResult->count);
                else
                    printf("Word not found\n");

                break;
            }
            case 3: {
                int n;
                printf("Enter number of most frequent words to display: ");
                scanf("%d", &n);
                getchar();

                displayMostFrequent(root, n);
                break;
            }
            case 4: {
                char* longest = longestWord(root);
                char* shortest = shortestWord(root);

                if (longest != NULL)
                    printf("Longest word: '%s'\n", longest);
                else
                    printf("No words found\n");

                if (shortest != NULL)
                    printf("Shortest word: '%s'\n", shortest);
                else
                    printf("No words found\n");

                break;
            }
            case 5: {
                char saveFilePath[100];
                printf("Enter file path to save tree: ");
                scanf("%99[^\n]", saveFilePath);
                getchar();

                FILE* saveFile = fopen(saveFilePath, "wb");
                if (saveFile == NULL) {
                    printf("Failed to open file for saving: %s\n", saveFilePath);
                    break;
                }

                saveTreeToFile(saveFile, root);

                fclose(saveFile);
                printf("Tree saved to file: %s\n", saveFilePath);
                break;
            }
            case 6: {
                char restoreFilePath[100];
                printf("Enter file path to restore tree: ");
                scanf("%99[^\n]", restoreFilePath);
                getchar();

                FILE* restoreFile = fopen(restoreFilePath, "rb");
                if (restoreFile == NULL) {
                    printf("Failed to open file for restoring: %s\n", restoreFilePath);
                    break;
                }

                Node* restoredTree = restoreTreeFromFile(restoreFile);
                if (restoredTree != NULL) {
                    freeTree(root);
                    root = restoredTree;
                    printf("Tree restored from file: %s\n", restoreFilePath);
                }
                else {
                    printf("Failed to restore tree from file: %s\n", restoreFilePath);
                }

                fclose(restoreFile);
                break;
            }
            case 7:
                freeTree(root);
                return SUCCESS;
            default:
                printf("Invalid option\n");
                break;
        }
    }
}