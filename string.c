#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


enum status_codes
{
    memory_allocation_problem,
    give_null_pointer,
    not_success,
    success
};


typedef struct String 
{
    char* data;  
    int length; 
} String;


bool is_null (const String * str)
{
    return (str == NULL || str -> data == NULL || str -> length == 0);
}

String create_string(const char* str, enum status_codes * status)
{
    int len = strlen(str);
    String new_string; new_string.data = NULL; new_string.data = 0;
    new_string.data = (char *)malloc((len + 1) * sizeof(char));
    if (new_string.data == NULL) {*status = memory_allocation_problem; return new_string;}
    new_string.length = len;
    strcpy(new_string.data, str);
    *status = success;
    return new_string;
}


enum status_codes delete_string(String * str) 
{
    if (is_null(str)) return give_null_pointer;
    free(str -> data);
    str -> data = NULL;
    str -> length = 0;
    return success;
}

enum status_codes equal(const String str1, const String str2, int * result) 
{
    if (is_null(&str1) || is_null(&str2)) return give_null_pointer;
    *result = strcmp(str1.data, str2.data);
    return success;
}

enum status_codes compare_strings(const String str1, const String str2, int * result) //проверить с NULL указат
{
    if (is_null(&str1) || is_null(&str2)) return give_null_pointer;
    int len1 = str1.length;
    int len2 = str2.length;

    if (len1 < len2) *result = -1;
    else if (len1 > len2) *result = 1;
    else equal(str1, str2, result);

    return success;
}

enum status_codes cpystr(String* dest, const String* src) 
{
    if (is_null(dest) || is_null(src)) return give_null_pointer;
    if (dest -> data != NULL) 
    {
        free(dest -> data);
    }

    dest -> length = src -> length;
    dest -> data = (char*)malloc((dest -> length + 1) * sizeof(char));
    if (dest -> data == NULL) return memory_allocation_problem;
    strcpy(dest -> data, src -> data);
    return success;
}

String* copy_to_new_string(const String src, enum status_codes * status) 
{
    String * new = NULL;
    new = (String *)malloc(sizeof(String));
    if (new == NULL) {*status = memory_allocation_problem; return new;}
    *new = create_string(src.data, status);
    if (*status != success) 
    {
        free(new);
        return NULL;
    }
    return new;
}


enum status_codes concatenate_strings(String* str1, const String* str2) 
{
    if (is_null(str1) || is_null(str2)) return give_null_pointer;
    char * result = NULL;
    int len = str1 -> length + str2 -> length;
    result = (char * )malloc((len + 1) * sizeof(char));
    if (result  == NULL) return memory_allocation_problem;
    
    strcpy(result, str1 -> data);
    strcat(result, str2->data);
    delete_string(str1);
    str1 -> data = result;
    str1 -> length = len;
    return success;
}


void print_status (enum status_codes status)
{
    switch (status)
    {
        case memory_allocation_problem:
            printf("Memory allocation problem\n");
            break;
        case give_null_pointer:
            printf("Error: Free null ptr\n");
            break;
        default:
            break;
    }
}
