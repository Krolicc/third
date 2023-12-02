#include "structs.h"
#include <time.h>

#define BUFFER_SIZE 512
#define MAIL_INCREMENT 10
#define MAIL_ID_SIZE 15


void free_mail(Mail * mail)
{
    if (mail->creation_time.data != NULL) free(mail->creation_time.data);
    if (mail->delivery_deadline.data != NULL) free(mail->delivery_deadline.data);
    if (mail->delivery_time.data != NULL) free(mail->delivery_time.data);
    if (mail->mail_id.data != NULL) free(mail->mail_id.data);
    if (mail->recipient.building.data != NULL) free(mail->recipient.building.data);
    if (mail->recipient.city.data != NULL) free(mail->recipient.city.data);
    if (mail->recipient.index.data != NULL) free(mail->recipient.index.data);
    if (mail->recipient.street.data != NULL) free(mail->recipient.street.data);
}


void print_mail(const Mail *mail)
{
    if (mail == NULL)
    {
        printf("Mail is NULL\n");
        return;
    }

    printf("\nГород получателя: %s\n", mail->recipient.city.data != NULL ? mail->recipient.city.data : "Unknown");
    printf("Улица получателя: %s\n", mail->recipient.street.data != NULL ? mail->recipient.street.data : "Unknown");
    printf("Номер дома получателя: %d\n", mail->recipient.house_number);
    printf("Корпус получателя: %s\n", mail->recipient.building.data != NULL ? mail->recipient.building.data : "Unknown");
    printf("Номер квартиры получателя: %d\n", mail->recipient.apartment_number);
    printf("Индекс получателя: %s\n", mail->recipient.index.data != NULL ? mail->recipient.index.data : "Unknown");
    printf("Вес посылки: %.2lf\n", mail->weight);
    printf("Почтовый идентификатор: %s\n", mail->mail_id.data != NULL ? mail->mail_id.data : "Unknown");
    printf("Время отправки: %s\n", mail->creation_time.data != NULL ? mail->creation_time.data : "Unknown");
    printf("Deadline: %s\n", mail->delivery_deadline.data != NULL ? mail->delivery_deadline.data : "Unknown");
    printf("Вручили: %s\n", mail->is_delivered ? "Да" : "Нет");
    printf("Время вручения: %s\n", mail->delivery_time.data != NULL ? mail->delivery_time.data : "Unknown");
    printf("\n");
}

void print_all_crnt(Post ** post)
{
    for (size_t i = 0; i < (*post)->mail_count; i++)
    {
        print_mail(&(*post)->mails[i]);
    }
}

bool is_number(const char *str)
{
    if (str == NULL || str[0] == '\0')
        return false;

    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] < '0' || str[i] > '9')
            return false;
    }

    return true;
}

char *get_current_time()
{
    time_t crnt_time;
    struct tm *info;
    char *buffer = (char *)malloc(20 * sizeof(char)); // 20 символов для формата "dd:MM:yyyy hh:mm:ss"

    if (buffer == NULL)
        return NULL;

    time(&crnt_time);
    info = localtime(&crnt_time);

    strftime(buffer, 20, "%d:%m:%Y %H:%M:%S", info);

    return buffer;
}

int compare_mail(const void* a, const void* b) 
{
    const Mail* mail_1 = (const Mail*)a;
    const Mail* mail_2 = (const Mail*)b;

    int status = strcmp(mail_1->recipient.index.data, mail_2->recipient.index.data);

    if (status != 0) 
    {
        return status;
    }

    return strcmp(mail_1->mail_id.data, mail_2->mail_id.data);
}

int compare_mail_for_3(const void* a, const void* b) 
{
    const Mail* mail_1 = (const Mail*)a;
    const Mail* mail_2 = (const Mail*)b;

    return strcmp(mail_1->creation_time.data, mail_2->creation_time.data);
}

enum status_codes add_mail_to_post(Post **post, Mail *mail)
{
    if (*post == NULL || mail == NULL)
    {
        return give_null_pointer;
    }

    if ((*post)->mail_count >= (*post)->mail_capacity)
    {
        size_t new_capacity = (*post)->mail_capacity + MAIL_INCREMENT;
        Mail *new_mail_array = (Mail *)realloc((*post)->mails, new_capacity * sizeof(Mail));

        if (new_mail_array == NULL)
        {
            return memory_allocation_problem;
        }

        (*post)->mails = new_mail_array;
        (*post)->mail_capacity = new_capacity;
    }

    (*post)->mails[(*post)->mail_count] = *mail;
    (*post)->mail_count++;
    qsort((*post)->mails, (*post)->mail_count, sizeof(Mail), compare_mail);
    return success;
}

enum status_codes search_mail(Post **post, char * mail_id, size_t * indx) 
{

    for (size_t i = 0; i < (*post)->mail_count; i++) 
    {
        if (strcmp((*post)->mails[i].mail_id.data, mail_id) == 0) 
        {
            if (indx != NULL) *indx = i;
            return success;
        }
    }
    return not_success;
}

enum status_codes search_mails(Post ** post, Mail ** delivered, Mail ** delivery_time_expired, size_t * count1, size_t * count2)
{
    (*delivered) = (Mail *)malloc(sizeof(Mail) * (*post)->mail_count);
    if (*delivered == NULL) return memory_allocation_problem;
    (*delivery_time_expired) = (Mail *)malloc(sizeof(Mail) * (*post)->mail_count);
    if (*delivery_time_expired == NULL) {free(*delivered);return  memory_allocation_problem;}

    size_t j = 0, k = 0;
    for (size_t i = 0; i < (*post)->mail_count; i++) 
    {
        if ((*post)->mails[i].is_delivered) 
        {
            (*delivered)[j] = (*post)->mails[i];
            j++;
        }
        else if (strcmp((*post)->mails[i].delivery_deadline.data, get_current_time()) < 0)
        {
            (*delivery_time_expired)[k] = (*post)->mails[i];
            k++;
        }
    }
    *count1 = j;
    *count2 = k;
    qsort((*delivered), j, sizeof(Mail), compare_mail_for_3);
    qsort((*delivery_time_expired), k, sizeof(Mail), compare_mail_for_3);
    return success;
}



void interactive(Post **post)
{
    while (1)
    {
        printf("Выберите действие:\n");
        printf("<1> Добавить почтовую отправку\n");
        printf("<2> Найти отправку. Если письмо доставленно, то установить время доставки\n");
        printf("<3> Поиск всех доставленных отправлений, и всех отправлений, срок доставки которых на текущий момент времени истёк\n");
        printf("<4> Вывести все текущие посылки\n");
        printf("<5> Выйти из программы\n");

        printf("-: ");
        char choice;
        scanf("%c", &choice);

        int c;
        while ((c = getchar()) != '\n' && c != EOF);

        char buffer[BUFFER_SIZE];
        enum status_codes status;
        size_t len;
        if (choice == '1')
        {
            Mail mail;
            printf("Введите данные для создания почтовой отправки:\n");

            printf("Город получателя: ");
            fgets(buffer, sizeof(buffer), stdin);
            len = strlen(buffer) - 1;
            buffer[len] = '\0';
            if (buffer[0] == '\0')
            {
                printf("Город не может быть пустым.\n");
                continue;
            }
            mail.recipient.city = create_string(buffer, &status);
            if (status != success) { print_status(status); break; }

            printf("Улица получателя: ");
            fgets(buffer, sizeof(buffer), stdin);
            len = strlen(buffer) - 1;
            buffer[len] = '\0';
            if (buffer[0] == '\0')
            {
                printf("Улица не может быть пустой.\n");
                continue;
            }
            mail.recipient.street = create_string(buffer, &status);
            if (status != success) { print_status(status); break; }

            printf("Номер дома получателя: ");
            if (scanf("%d", &mail.recipient.house_number) != 1 || mail.recipient.house_number <= 0)
            {
                printf("Номер дома должен быть натуральным числом.\n");
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Корпус получателя: ");
            fgets(buffer, sizeof(buffer), stdin);
            len = strlen(buffer) - 1;
            buffer[len] = '\0';
            mail.recipient.building = create_string(buffer, &status);
            if (status != success) { print_status(status); break; }

            printf("Номер квартиры получателя: ");
            if (scanf("%d", &mail.recipient.apartment_number) != 1 || mail.recipient.apartment_number <= 0)
            {
                printf("Номер квартиры должен быть натуральным числом.\n");
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Индекс получателя: ");
            fgets(buffer, sizeof(buffer), stdin); 
            len = strlen(buffer) - 1;
            buffer[len] = '\0';
            if (len != 6 || !is_number(buffer))
            {
                printf("Индекс должен содержать шесть цифр.\n");
                continue;
            }
            mail.recipient.index = create_string(buffer, &status);
            if (status != success) { print_status(status); break; }

            printf("Вес посылки: ");
            if (scanf("%lf", &mail.weight) != 1 || mail.weight < 0)
            {
                printf("Вес посылки должен быть неотрицательным вещественным числом.\n");
                while ((c = getchar()) != '\n' && c != EOF);
                continue;
            }
            while ((c = getchar()) != '\n' && c != EOF);

            printf("Почтовый идентификатор: ");
            fgets(buffer, sizeof(buffer), stdin);
            len = strlen(buffer) - 1;
            buffer[len] = '\0';
            if (len != 14 || !is_number(buffer))
            {
                printf("Почтовый идентификатор должен содержать 14 цифр.\n");
                continue;
            }
            if (search_mail(post, buffer, NULL) == success)
            {
                printf("Такой почтовый идентификатор уже существует\n");
                continue;
            }
            mail.mail_id = create_string(buffer, &status);
            if (status != success) { print_status(status); break; }

            mail.creation_time = create_string(get_current_time(), &status);
            if (status != success) { print_status(status); break; }

            mail.is_delivered = false;
            mail.delivery_time.data = NULL;

            struct tm tm;
//            strptime(mail.creation_time.data, "%d:%m:%Y %H:%M:%S", &tm);
            
            time_t current_time_t = mktime(&tm);
            time_t delivery_seconds;
            if (mail.weight <= 1.0) 
            {
                delivery_seconds = 30;
            } 
            else if (mail.weight <= 5.0) 
            {
                delivery_seconds = 70;
            } 
            else 
            {
                delivery_seconds = 120;
            }
            time_t delivery_deadline_t = current_time_t + delivery_seconds;
            struct tm *info = localtime(&delivery_deadline_t);
            char deadline_buffer[20];
            strftime(deadline_buffer, 20, "%d:%m:%Y %H:%M:%S", info);
            mail.delivery_deadline = create_string(deadline_buffer, &status);
            if (status != success) { print_status(status); break; }
            
            status = add_mail_to_post(post, &mail);
            if (status != success) { print_status(status); break; }
            printf("\nПочтовая отправка добавлена\n\n");
        }
        else if (choice == '2')
        {
            printf("Введите идентификатор почтовой отправки: ");
            char mail_id[MAIL_ID_SIZE];
            scanf("%s", mail_id);
            while ((c = getchar()) != '\n' && c != EOF);
            size_t indx;
            status = search_mail(post, mail_id, &indx);
            if (status == not_success) 
            {
                printf("Почтовая отправка с этим идентификатором не найдена.\n");
            } 
            else if (status == success) 
            {
                printf("Почтовая отправка с данным идентификатором найдена\n");
                char response;
                printf("Хотите записать время доставки (r), удалить посылку (d) или напечатать информацию о ней (p)? (e) - выйти\n");
                while(1)
                {
                    printf("-: ");
                    scanf(" %c", &response);
                    while ((c = getchar()) != '\n' && c != EOF);
                    if (response == 'r') 
                    {
                        if ((*post)->mails[indx].is_delivered) printf("Эта посылка уже доставлена\n");
                        else 
                        {
                            (*post)->mails[indx].is_delivered = true;
                            (*post)->mails[indx].delivery_time.data = get_current_time();
                            printf("Посылка доставлена\n");
                        }
                    } 
                    else if (response == 'p') 
                    {
                        print_mail(&(*post)->mails[indx]);
                    }
                    else if (response == 'd')
                    {
                        free_mail(&(*post)->mails[indx]);
                        for (size_t j = indx; j < (*post)->mail_count - 1; j++)
                        {
                            (*post)->mails[j] = (*post)->mails[j + 1];
                        }
                        (*post)->mail_count--;
                        printf("Почтовая отправка удалена.\n");
                        break;
                    }
                    else if (response == 'e') break;
                    else 
                    {
                        printf("Неверный выбор. Время доставки не записано.\n");
                    }
                } 
            }
            else 
            {
                print_status(status);
                break;
            }
        }
        else if (choice == '3')
        {
            Mail * delivered = NULL;
            Mail * delivery_time_expired = NULL;
            size_t count1 = 0, count2 = 0;
            status = search_mails(post, &delivered, &delivery_time_expired, &count1, &count2);
            if (status == success)
            {
                printf("\nДоставленные посылки: ");
                if (count1 == 0) printf("таких посылок нет\n");
                else {for (size_t i = 0; i < count1; i++) print_mail(&(delivered[i]));}
                printf("\nПосылки с просроченной доставкой: ");
                if (count2 == 0) printf("таких посылок нет\n\n");
                else {for (size_t i = 0; i < count2; i++) print_mail(&(delivery_time_expired[i]));}
                free(delivery_time_expired);
                free(delivered);
            }
            else print_status(status);
        }
        else if (choice == '4')
        {
            if ((*post)->mail_count == 0) printf("Посылок нет\n");
            else
            {
                printf("Все текущие посылки:\n");
                print_all_crnt(post);
            }
        }
        else if (choice == '5')
        {
            printf("Goodbye!\n");
            break;
        }
        else
        {
            printf("Неверный выбор. Пожалуйста, выберите действие от 1 до 4.\n");
        }
    }
}
