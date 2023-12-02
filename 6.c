#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <math.h>

#define MAX_SIZE 100

typedef enum {
    SUCCESS = 0,
    INCORRECT_INPUT_ARGUMENTS,
    INCORRECT_COUNT_ARGUMENTS,
    OVERFLOW_ERROR,
    FILE_OPENING_ERROR,
    FILE_READING_ERROR,
    FILE_CLOSING_ERROR,
    MEMORY_ALLOCATION_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Некорректный ввод аргументов, попробуйте ещё раз",
        "Некорректный кол-во аргументов, попробуйте ещё раз",
        "Произошло переполнение",
        "Не удалось открыть файл",
        "Файл прочитан не полностью",
        "Ошибка закрытия файла",
        "Ошибка выделения памяти",
        "Неизвестная ошибка, что-то пошло не так"
};

typedef struct Stop {
    char vehicleNumber[MAX_SIZE];
    int arrivalYear;
    int arrivalMonth;
    int arrivalDay;
    int arrivalHour;
    int arrivalMinute;
    int arrivalSecond;
    int departureYear;
    int departureMonth;
    int departureDay;
    int departureHour;
    int departureMinute;
    int departureSecond;
    char marker[MAX_SIZE];
    char coordinate[MAX_SIZE];
    struct Stop* next;
} Stop;

typedef struct Route {
    int count;
    struct Stop* firstStop;
    struct Route* next;
} Route;

void freeStop(Stop* head) {
    Stop* current = head;
    Stop* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }
}

void freeRoutes(Route* head) {
    Route* current = head;
    Route* next;

    while (current != NULL) {
        next = current->next;
        freeStop(current->firstStop);
        free(current);
        current = next;
    }
}

int compareByDate(struct Stop* stopA, struct Stop* stopB) {
    if(stopA->arrivalYear == stopB->arrivalYear) {
        if(stopA->arrivalMonth == stopB->arrivalMonth) {
            if(stopA->arrivalDay == stopB->arrivalDay) {
                if (stopA->arrivalHour == stopB->arrivalHour) {
                    if (stopA->arrivalMinute == stopB->arrivalMinute)
                        return stopA->arrivalSecond < stopB->arrivalYear;
                    else return stopA->arrivalMinute < stopB->arrivalMinute;
                } else return stopA->arrivalHour < stopB->arrivalHour;
            } else return stopA->arrivalDay < stopB->arrivalDay;
        } else return stopA->arrivalMonth < stopB->arrivalMonth;
    } else return stopA->arrivalYear < stopB->arrivalYear;
}

Stop* createStop() {
    Stop* newStop = (Stop*)malloc(sizeof(Stop));

    if (newStop == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }

    newStop->next = NULL;
    newStop->arrivalYear = 1000000;
    newStop->arrivalMonth = 13;
    newStop->arrivalDay = 32;
    newStop->arrivalHour = 25;
    newStop->arrivalMinute = 61;
    newStop->arrivalSecond = 61;
    newStop->departureYear = 1000000;
    newStop->departureMonth = 13;
    newStop->departureDay = 32;
    newStop->departureHour = 25;
    newStop->departureMinute = 61;
    newStop->departureSecond = 61;

    return newStop;
}

Route* createRoute() {
    Route* newRoute = (Route*)malloc(sizeof(Route));

    if (newRoute == NULL) {
        printf("%s", errorMessages[MEMORY_ALLOCATION_ERROR]);
        exit(MEMORY_ALLOCATION_ERROR);
    }

    newRoute->firstStop = NULL;
    newRoute->next = NULL;

    return newRoute;
}

void insertRoute(struct Route** routes, struct Route* route) {
    struct Route* current = (*routes);
    struct Route* previous = NULL;

    while (current != NULL) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        route->next = current;
        (*routes) = route;
    } else {
        route->next = current;
        previous->next = route;
    }
}

void insertStop(struct Route** route, struct Stop* stop) {
//    printf("%d %s\n", (*route)->count, stop->vehicleNumber);
    struct Stop* previous = NULL;
    struct Route* extraRoute = (*route);

    while(extraRoute) {
        if(strcmp((*route)->firstStop->vehicleNumber, stop->vehicleNumber) == 0 || (*route)->count == 0)
            break;

        if(extraRoute->next == NULL) {
            insertRoute(&extraRoute, createRoute());
            extraRoute = (*route)->next;extraRoute->count = 0;
            break;
        }

        extraRoute = extraRoute->next;
    }
    if(extraRoute->firstStop == NULL) extraRoute->firstStop = createStop();
    struct Stop* current = extraRoute->firstStop;

    while (current != NULL && compareByDate(current, stop) == 1) {
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        stop->next = current;
        extraRoute->firstStop = stop;
    } else {
        stop->next = current;
        previous->next = stop;
    }

    (extraRoute)->count++;
}

void printRoutes(struct Route* routes) {
    struct Route* currentRoute = routes;

    while (currentRoute != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;

        printf("Route: ");
        while (currentStop != NULL) {
            printf("%s ", currentStop->marker);
            currentStop = currentStop->next;
        }
        printf("\n");

        currentRoute = currentRoute->next;
    }
}

void searchMostVisited(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* mostVisitedRoute = currentRoute;
    int mostVisitedRoutesCount = 0;

    while (currentRoute != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;

        int currentRouteStopsCount = 0;
        int isRouteCompleted = 0;
        while (currentStop != NULL) {
            if(strcmp("ST",currentStop->marker) == 0) {
                isRouteCompleted = 1;
            } else if (strcmp("EN",currentStop->marker) == 0 && isRouteCompleted == 1) {
                currentRouteStopsCount++;
            }
            currentStop = currentStop->next;
        }

        if (currentRouteStopsCount > mostVisitedRoutesCount) {
            mostVisitedRoutesCount = currentRouteStopsCount;
            mostVisitedRoute = currentRoute;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наибольший кол-во завершенных маршрутов у маршрута %s: %d\n", mostVisitedRoute->firstStop->vehicleNumber, mostVisitedRoutesCount);
}

void searchLeastVisited(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* leastVisitedRoute = currentRoute;
    int leastVisitedRoutesCount = 100000;

    while (currentRoute != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;

        int currentRouteStopsCount = 0;
        int isRouteCompleted = 0;
        while (currentStop != NULL) {
            if(strcmp("ST",currentStop->marker) == 0) {
                isRouteCompleted = 1;
            } else if (strcmp("EN",currentStop->marker) == 0 && isRouteCompleted == 1) {
                currentRouteStopsCount++;
                isRouteCompleted = 0;
            }
            currentStop = currentStop->next;
        }

        if (currentRouteStopsCount < leastVisitedRoutesCount) {
            leastVisitedRoutesCount = currentRouteStopsCount;
            leastVisitedRoute = currentRoute;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наименьшее кол-во завершенных маршрутов у маршрута %s: %d\n", leastVisitedRoute->firstStop->vehicleNumber, leastVisitedRoutesCount);
}

void searchLongestRoute(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* longestRoute = currentRoute;
    double longestRouteDistance = 0;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        double currentRouteDistance = 0;

        int isRouteCompleted = 0;
        while (currentStop->next != NULL) {
            if(strcmp("ST",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0 ||
               strcmp("ST",currentStop->marker) == 0 && strcmp("IN",currentStop->next->marker) == 0 ||
               strcmp("IN",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0) {
                int x1, y1, x2, y2;

                sscanf(currentStop->coordinate, "%d%*c%d", &x1, &y1);
                sscanf(currentStop->next->coordinate, "%d%*c%d", &x2, &y2);

                currentRouteDistance += sqrt(pow(abs(x1-x2), 2) + pow(abs(y1-y2), 2));
            }

            if(strcmp("ST",currentStop->marker) == 0) {
                isRouteCompleted = 1;
            } else if (strcmp("EN",currentStop->marker) == 0 && isRouteCompleted == 1) {
                if (currentRouteDistance > longestRouteDistance) {
                    longestRouteDistance = currentRouteDistance;
                    longestRoute = currentRoute;
                }
                currentRouteDistance = 0;
                isRouteCompleted = 0;
            }

            currentStop = currentStop->next;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наибольшая длина маршрута у маршрута %s: %lf\n", longestRoute->firstStop->vehicleNumber, longestRouteDistance);
}

void searchShortestRoute(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* shortestRoute = currentRoute;
    double shortestRouteDistance = 100000;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        double currentRouteDistance = 0;

        int isRouteCompleted = 0;
        while (currentStop->next != NULL) {
            if(strcmp("ST",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0 ||
               strcmp("ST",currentStop->marker) == 0 && strcmp("IN",currentStop->next->marker) == 0 ||
               strcmp("IN",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0) {
                int x1, y1, x2, y2;

                sscanf(currentStop->coordinate, "%d%*c%d", &x1, &y1);
                sscanf(currentStop->next->coordinate, "%d%*c%d", &x2, &y2);

                currentRouteDistance += sqrt(pow(abs(x1-x2), 2) + pow(abs(y1-y2), 2));
            }

            if(strcmp("ST",currentStop->marker) == 0) {
                isRouteCompleted = 1;
            } else if (strcmp("EN",currentStop->marker) == 0 && isRouteCompleted == 1) {
                if (currentRouteDistance < shortestRouteDistance) {
                    shortestRouteDistance = currentRouteDistance;
                    shortestRoute = currentRoute;
                }
                currentRouteDistance = 0;
                isRouteCompleted = 0;
            }

            currentStop = currentStop->next;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наименьшая длина маршрута у маршрута %s: %lf\n", shortestRoute->firstStop->vehicleNumber, shortestRouteDistance);
}

void searchLongestDistance(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* longestRoute = currentRoute;
    double longestRouteDistance = 0;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        double currentRouteDistance = 0;

        while (currentStop->next != NULL) {
            if(strcmp("ST",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0 ||
               strcmp("ST",currentStop->marker) == 0 && strcmp("IN",currentStop->next->marker) == 0 ||
               strcmp("IN",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0) {
                int x1, y1, x2, y2;

                sscanf(currentStop->coordinate, "%d%*c%d", &x1, &y1);
                sscanf(currentStop->next->coordinate, "%d%*c%d", &x2, &y2);

                currentRouteDistance += sqrt(pow(abs(x1-x2), 2) + pow(abs(y1-y2), 2));
            }

            currentStop = currentStop->next;
        }

        if (currentRouteDistance > longestRouteDistance) {
            longestRouteDistance = currentRouteDistance;
            longestRoute = currentRoute;
        }
        currentRoute = currentRoute->next;
    }

    printf("Наибольшая длина пройденного расстояния у маршрута %s: %lf\n", longestRoute->firstStop->vehicleNumber, longestRouteDistance);
}

void searchShortestDistance(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* shortestRoute = currentRoute;
    double shortestRouteDistance = 100000;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        double currentRouteDistance = 0;

        while (currentStop->next != NULL) {
            if(strcmp("ST",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0 ||
               strcmp("ST",currentStop->marker) == 0 && strcmp("IN",currentStop->next->marker) == 0 ||
               strcmp("IN",currentStop->marker) == 0 && strcmp("EN",currentStop->next->marker) == 0) {
                int x1, y1, x2, y2;

                sscanf(currentStop->coordinate, "%d%*c%d", &x1, &y1);
                sscanf(currentStop->next->coordinate, "%d%*c%d", &x2, &y2);

                currentRouteDistance += sqrt(pow(abs(x1-x2), 2) + pow(abs(y1-y2), 2));
            }

            currentStop = currentStop->next;
        }
        if (currentRouteDistance < shortestRouteDistance) {
            shortestRouteDistance = currentRouteDistance;
            shortestRoute = currentRoute;
        }
        currentRoute = currentRoute->next;
    }

    printf("Наименьшая длина пройденного расстояния у маршрута %s: %lf\n", shortestRoute->firstStop->vehicleNumber, shortestRouteDistance);
}

void searchLongestStop(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* longestStop = currentRoute;
    int longestStopTime = 0;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        int currentStopTime = 0;

        while (currentStop->next != NULL) {
            currentStopTime = (currentStop->departureYear - currentStop->arrivalYear) * 365 * 24 * 60 * 60;
            currentStopTime += (currentStop->departureMonth - currentStop->arrivalMonth) * 30 * 24 * 60 * 60;
            currentStopTime += (currentStop->departureDay - currentStop->arrivalDay) * 24 * 60 * 60;
            currentStopTime += (currentStop->departureHour - currentStop->arrivalHour) * 60 * 60;
            currentStopTime += (currentStop->departureMinute - currentStop->arrivalMinute) * 60;
            currentStopTime += currentStop->departureSecond - currentStop->arrivalSecond;

            if (currentStopTime > longestStopTime) {
                longestStopTime = currentStopTime;
                longestStop = currentRoute;
            }
            currentStop = currentStop->next;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наибольшая по продолжительности остановка у маршрута %s: %d s\n", longestStop->firstStop->vehicleNumber, longestStopTime);
}

void searchShortestStop(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* shortestStop = currentRoute;
    long long shortestStopTime = 1000000000000000000;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        long long currentStopTime = 0;

        while (currentStop->next != NULL) {
            currentStopTime = (currentStop->departureYear - currentStop->arrivalYear) * 365 * 24 * 60 * 60;
            currentStopTime += (currentStop->departureMonth - currentStop->arrivalMonth) * 30 * 24 * 60 * 60;
            currentStopTime += (currentStop->departureDay - currentStop->arrivalDay) * 24 * 60 * 60;
            currentStopTime += (currentStop->departureHour - currentStop->arrivalHour) * 60 * 60;
            currentStopTime += (currentStop->departureMinute - currentStop->arrivalMinute) * 60;
            currentStopTime += currentStop->departureSecond - currentStop->arrivalSecond;

            if (currentStopTime < shortestStopTime) {
                shortestStopTime = currentStopTime;
                shortestStop = currentRoute;
            }
            currentStop = currentStop->next;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наименьшая по продолжительности остановка у маршрута %s: %lld s\n", shortestStop->firstStop->vehicleNumber, shortestStopTime);
}

void searchLongestIdleTime(struct Route* routes) {
    struct Route* currentRoute = routes;
    struct Route* longestStop = currentRoute;
    int longestStopTime = 0;

    while (currentRoute->next != NULL) {
        struct Stop* currentStop = currentRoute->firstStop;
        int currentStopTime = 0;

        while (currentStop->next != NULL) {
            currentStopTime += (currentStop->departureYear - currentStop->arrivalYear) * 365 * 24 * 60 * 60;
            currentStopTime += (currentStop->departureMonth - currentStop->arrivalMonth) * 30 * 24 * 60 * 60;
            currentStopTime += (currentStop->departureDay - currentStop->arrivalDay) * 24 * 60 * 60;
            currentStopTime += (currentStop->departureHour - currentStop->arrivalHour) * 60 * 60;
            currentStopTime += (currentStop->departureMinute - currentStop->arrivalMinute) * 60;
            currentStopTime += currentStop->departureSecond - currentStop->arrivalSecond;

            currentStop = currentStop->next;
        }

        if (currentStopTime > longestStopTime) {
            longestStopTime = currentStopTime;
            longestStop = currentRoute;
        }

        currentRoute = currentRoute->next;
    }

    printf("Наибольший простой у маршрута %s: %d s\n", longestStop->firstStop->vehicleNumber, longestStopTime);
}

int main(int argc, char* argv[]) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    if(argc < 2) {
        printf("%s\n", errorMessages[INCORRECT_COUNT_ARGUMENTS]);
        return INCORRECT_COUNT_ARGUMENTS;
    }

    Route* RootRoute = createRoute();
    RootRoute->firstStop = createStop();
    RootRoute->count = 0;

    for (int i = 1; i < argc; i++) {
        FILE* file = fopen(argv[i], "r");
        if (file != NULL) {
            char arg[80], coordinate[80], dateArrive[100], dateDeparture[100], marker[100], vehicleNumber[100];
            int count = 0;

            fscanf(file, "%s", coordinate);
            if(ferror(file)) {
                printf("%s\n", errorMessages[FILE_READING_ERROR]);
                free(RootRoute);
                return FILE_READING_ERROR;
            }
            int isArriveData = 1;
            while (fscanf(file, "%s", arg) == 1) {
                if(ferror(file)) {
                    printf("%s\n", errorMessages[FILE_READING_ERROR]);
                    free(RootRoute);
                    return FILE_READING_ERROR;
                }
                count++;
                if(strcmp("ST", arg) == 0 || strcmp("EN", arg) == 0 || strcmp("IN", arg) == 0) {
                    strcpy(marker, arg);
                } else if (strchr(".", arg[2]) != NULL) {
                    if(isArriveData) {
                        strcpy(dateArrive, arg);
                        fscanf(file, "%s", arg);
                        sprintf(dateArrive, "%s %s", dateArrive, arg);
                        isArriveData = 0;
                    } else {
                        isArriveData = 1;
                        int isArriveMoreThenDeparture = 0;
                        strcpy(dateDeparture, arg);
                        fscanf(file, "%s", arg);
                        sprintf(dateDeparture, "%s %s", dateDeparture, arg);
                        for(int j=9; j > -1; j--) {
                            if(dateDeparture[j] < dateArrive[j]) {
                                strcpy(arg, dateDeparture);
                                strcpy(dateDeparture, dateArrive);
                                strcpy(dateArrive, arg);
                                break;
                            } else if (dateDeparture[j] > dateArrive[j]) {
                                isArriveMoreThenDeparture = 1;
                                break;
                            }
                        }

                        for(int j=10; j < strlen(dateArrive); j++) {
                            if(isArriveMoreThenDeparture == 1 || dateDeparture[j] > dateArrive[j]) {
                                break;
                            } else if(dateDeparture[j] < dateArrive[j]) {
                                strcpy(arg, dateDeparture);
                                strcpy(dateDeparture, dateArrive);
                                strcpy(dateArrive, arg);
                                break;
                            }
                        }
                    }
                } else {
                    strcpy(vehicleNumber, arg);
                }

                if (count == 4){

                    Stop *extraStop = createStop();

                    extraStop->arrivalDay = (dateArrive[0] - '0') * 10 + (dateArrive[1] - '0');
                    extraStop->arrivalMonth = (dateArrive[3] - '0') * 10 + (dateArrive[4]- '0');
                    extraStop->arrivalYear = (dateArrive[6] - '0') * 1000 + (dateArrive[7]- '0') * 100 +
                            (dateArrive[8] - '0') * 10 + (dateArrive[9] - '0');
                    extraStop->arrivalHour = (dateArrive[11] - '0') * 10 + (dateArrive[12]- '0');
                    extraStop->arrivalMinute = (dateArrive[14] - '0') * 10 + (dateArrive[15]- '0');
                    extraStop->arrivalSecond = (dateArrive[17] - '0') * 10 + (dateArrive[18]- '0');
                    extraStop->departureDay = (dateDeparture[0] - '0') * 10 + (dateDeparture[1]- '0');
                    extraStop->departureMonth = (dateDeparture[3] - '0') * 10 + (dateDeparture[4]- '0');
                    extraStop->departureYear = (dateDeparture[6] - '0') * 1000 + (dateDeparture[7]- '0') * 100 +
                            (dateDeparture[8] - '0') * 10 + (dateDeparture[9]- '0');
                    extraStop->departureHour = (dateDeparture[11] - '0') * 10 + (dateDeparture[12] - '0');
                    extraStop->departureMinute = (dateDeparture[14] - '0') * 10 + (dateDeparture[15]- '0');
                    extraStop->departureSecond = (dateDeparture[17] - '0') * 10 + (dateDeparture[18]- '0');

                    strcpy(extraStop->vehicleNumber, vehicleNumber);
                    strcpy(extraStop->marker, marker);
                    strcpy(extraStop->coordinate, coordinate);

                    insertStop(&RootRoute, extraStop);
                    strcmp(dateArrive, "");
                    count = 0;
                }
            }

            if ( fclose (file) == EOF) {
                printf ("%s", errorMessages[FILE_CLOSING_ERROR]);
                return FILE_CLOSING_ERROR;
            }



        } else {
            printf("%s\n", errorMessages[FILE_OPENING_ERROR]);
            free(RootRoute);
            return FILE_OPENING_ERROR;
        }

    }

    printf("Введите цифру, соответсвующую одной из опций:\n\n");
    printf("Транспортного средство, которое проехало:\n");
    printf("  1. Больше всех маршрутов;\n");
    printf("  2. Меньше всех маршрутов;\n");
    printf("  3. Самый длинный маршрут;\n");
    printf("  4. Самый короткий маршрут;\n");
    printf("  5. Самое большое расстояние;\n");
    printf("  6. Самое маленькое расстояние;\n");
    printf("7. Транспортного средство с самой долгой остановкой;\n");
    printf("8. Транспортного средство с самой короткой остановкой;\n");
    printf("9. Транспортного средство с самым долгим простоем;\n");
    printf("0. Выйти.\n\n");

    int user_input = -1;
    char numString[90];

    while(user_input != 0) {

        printf("Введите номер опции: ");
        scanf("%s", numString);
        if (strchr("-0123456789", numString[0]) == NULL) {
            printf("Неверный выбор.\n");
            continue;
        }

        user_input = atoi(numString);

        switch (user_input) {
            case 1:
                searchMostVisited(RootRoute);
                break;
            case 2:
                searchLeastVisited(RootRoute);
                break;
            case 3:
                searchLongestRoute(RootRoute);
                break;
            case 4:
                searchShortestRoute(RootRoute);
                break;
            case 5:
                searchLongestDistance(RootRoute);
                break;
            case 6:
                searchShortestDistance(RootRoute);
                break;
            case 7:
                searchLongestStop(RootRoute);
                break;
            case 8:
                searchShortestStop(RootRoute);
                break;
            case 9:
                searchLongestIdleTime(RootRoute);
                break;
            case 0:
                break;
            default:
                printf("Неверный выбор!\n");
                break;
        }

    }

    freeRoutes(RootRoute);
    return SUCCESS;
}
