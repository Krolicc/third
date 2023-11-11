#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int coefficient;
    int power;
    struct Node* next;
} Node;

Node* createNode(int coefficient, int power) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->coefficient = coefficient;
    newNode->power = power;
    newNode->next = NULL;
    return newNode;
}

void insertTerm(Node** head, int coefficient, int power) {
    Node* newNode = createNode(coefficient, power);
    newNode->next = *head;
    *head = newNode;
}

void displayPolynomial(Node* head) {
    Node* current = head;
    while (current != NULL) {
        if (current->coefficient != 0) {
            printf("%+dx^%d ", current->coefficient, current->power);
        }
        current = current->next;
    }
    printf("\n");
}

Node* addPolynomials(Node* poly1, Node* poly2) {
    Node* result = NULL;
    while (poly1 != NULL && poly2 != NULL) {
        if (poly1->power > poly2->power) {
            insertTerm(&result, poly1->coefficient, poly1->power);
            poly1 = poly1->next;
        }
        else if (poly1->power < poly2->power) {
            insertTerm(&result, poly2->coefficient, poly2->power);
            poly2 = poly2->next;
        }
        else {
            int sum = poly1->coefficient + poly2->coefficient;
            if (sum != 0) {
                insertTerm(&result, sum, poly1->power);
            }
            poly1 = poly1->next;
            poly2 = poly2->next;
        }
    }

    while (poly1 != NULL) {
        insertTerm(&result, poly1->coefficient, poly1->power);
        poly1 = poly1->next;
    }

    while (poly2 != NULL) {
        insertTerm(&result, poly2->coefficient, poly2->power);
        poly2 = poly2->next;
    }

    return result;
}

Node* subtractPolynomials(Node* poly1, Node* poly2) {
    Node* negPoly2 = NULL;
    Node* current = poly2;

    while (current != NULL) {
        insertTerm(&negPoly2, -current->coefficient, current->power);
        current = current->next;
    }

    Node* result = addPolynomials(poly1, negPoly2);
    freePolynomial(negPoly2);
    return result;
}

Node* multiplyPolynomials(Node* poly1, Node* poly2) {
    Node* result = NULL;
    Node* current1 = poly1;

    while (current1 != NULL) {
        Node* current2 = poly2;
        while