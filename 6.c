#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char number[20];
    char arrivalTime[20];
    char departureTime[20];
    char marker[10];
} StopPoint;

typedef struct Route {
    StopPoint stopPoint;
    struct Route *next;
} Route;

typedef struct {
    char startPoint[50];
    char endPoint[50];
    Route *routes;
} Vehicle;

typedef struct List {
    Vehicle vehicle;
    struct List *next;
} List;

typedef struct {
    List *head;
} Storage;

void initializeStorage(Storage *storage) {
    storage->head = NULL;
}

void addStopPoint(Storage *storage, StopPoint stopPoint, char *startPoint, char *endPoint) {
    List *currentVehicle = storage->head;
    while (currentVehicle != NULL) {
        if (strcmp(currentVehicle->vehicle.startPoint, startPoint) == 0 &&
            strcmp(currentVehicle->vehicle.endPoint, endPoint) == 0) {
            Route *currentRoute = currentVehicle->vehicle.routes;
            Route *previousRoute = NULL;

            while (currentRoute != NULL) {
                if (strcmp(currentRoute->stopPoint.arrivalTime, stopPoint.arrivalTime) > 0) {
                    break;
                }

                previousRoute = currentRoute;
                currentRoute = currentRoute->next;
            }

            Route *newRoute = malloc(sizeof(Route));
            if (newRoute == NULL) {
                printf("Error allocating memory.\n");
                return;
            }

            newRoute->stopPoint = stopPoint;
            newRoute->next = currentRoute;

            if (previousRoute != NULL) {
                previousRoute->next = newRoute;
            } else {
                currentVehicle->vehicle.routes = newRoute;
            }

            return;
        }

        currentVehicle = currentVehicle->next;
    }

    // Vehicle not found, add a new one
    List *newVehicle = malloc(sizeof(List));
    if (newVehicle == NULL) {
        printf("Error allocating memory.\n");
        return;
    }

    strcpy(newVehicle->vehicle.startPoint, startPoint);
    strcpy(newVehicle->vehicle.endPoint, endPoint);

    Route *newRoute = malloc(sizeof(Route));
    if (newRoute == NULL) {
        printf("Error allocating memory.\n");
        free(newVehicle);
        return;
    }

    newRoute->stopPoint = stopPoint;
    newRoute->next = NULL;

    newVehicle->vehicle.routes = newRoute;
    newVehicle->next = storage->head;
    storage->head = newVehicle;
}

void searchMostVisitedVehicle(Storage *storage) {
    List *currentVehicle = storage->head;
    int maxRoutes = 0;
    char mostVisitedVehicle[20] = "";

    while (currentVehicle != NULL) {
        int numRoutes = 0;
        Route *currentRoute = currentVehicle->vehicle.routes;

        while (currentRoute != NULL) {
            numRoutes++;
            currentRoute = currentRoute->next;
        }

        if (numRoutes > maxRoutes) {
            maxRoutes = numRoutes;
            strcpy(mostVisitedVehicle, currentVehicle->vehicle.number);
        }

        currentVehicle = currentVehicle->next;
    }

    if (maxRoutes > 0) {
        printf("Most visited vehicle: %s\n", mostVisitedVehicle);
    } else {
        printf("No vehicles found.\n");
    }
}

// Реализуйте остальные функции для поиска информации

int main(int argc, char *argv[]) {
    Storage storage;
    initializeStorage(&storage);

    // Прочитайте файлы и добавьте данные в хранилище
    // Пример чтения файла:
    // FILE *file = fopen(path, "r");
    // if (file == NULL) {
    //     printf("Error opening file.\n");
    //     return 1;
    // }
    // char vehicle[20];
    // char startPoint[50];
    // char endPoint[50];
    // char arrivalTime[20];
    // char departureTime[20];
    // char marker[10];
    // while (fscanf(file, "%s %s %s %s %s %s", vehicle, startPoint, endPoint, arrivalTime, departureTime, marker) == 6) {
    //     StopPoint stopPoint;
    //     strcpy(stopPoint.number, vehicle);
    //     strcpy(stopPoint.arrivalTime, arrivalTime);
    //     strcpy(stopPoint.departureTime, departureTime);
    //     strcpy(stopPoint.marker, marker);
    //     addStopPoint(&storage, stopPoint, startPoint, endPoint);
    // }
    // fclose(file);

    // Интерактивный диалог
    int choice;
    while (1) {
        printf("1. Search most visited vehicle\n");
        printf("2. Search vehicle with longest route\n");
        printf("3. Search vehicle with shortest route\n");
        printf("4. Search vehicle with longest stop\n");
        printf("5. Search vehicle with shortest stop\n");
        printf("6. Search vehicle with longest idle time\n");
        printf("7. Search vehicle with shortest idle time\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 0:
                return 0;
            case 1:
                searchMostVisitedVehicle(&storage);
                break;
            // Добавьте обработку остальных вариантов выбора
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }

        printf("\n");
    }

    return 0;
}