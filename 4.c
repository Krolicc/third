#include "mail.c"
#include <Windows.h>

typedef enum {
    SUCCESS = 0,
    OVERFLOW_ERROR,
    MEMORY_ALLOCATION_ERROR,
    UNKNOWN_ERROR
} ErrorCode;

static const char* errorMessages[] = {
        "Всё хорошо",
        "Произошло переполнение",
        "Проблемы с выделением памяти",
        "Неизвестная ошибка, что-то пошло не так"
};

int main() 
{
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);


    Post *post = NULL;
    post = (Post * )malloc(sizeof(Post) * MAIL_INCREMENT);
    if (post == NULL)
    {
        perror(errorMessages[MEMORY_ALLOCATION_ERROR]);
        return MEMORY_ALLOCATION_ERROR;
    }

    post->mails = (Mail *)malloc(MAIL_INCREMENT * sizeof(Mail));
    if (post->mails == NULL)
    {
        free(post);
        perror(errorMessages[MEMORY_ALLOCATION_ERROR]);
        return MEMORY_ALLOCATION_ERROR;
    }
    post->mail_capacity = MAIL_INCREMENT;
    post->mail_count = 0;
    //указать адрес post
    interactive(&post);


    for (size_t i = 0; i < post->mail_count; i++)
    {
        free_mail(&(post->mails[i]));
    }
    free(post->mails);
    free(post);

    return SUCCESS;
}
