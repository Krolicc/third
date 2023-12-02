#ifndef STRUCTS_H
#define STRUCTS_H

#include "string.c"

typedef struct Address 
{
    String city;
    String street;
    int house_number;
    String building;
    int apartment_number;
    String index;
} Address;

typedef struct Mail 
{
    Address recipient;
    double weight;
    String mail_id;
    String creation_time;
    String delivery_deadline;
    String delivery_time;
    bool is_delivered; // ? 
} Mail;

typedef struct Post 
{
    Address * post_office_address;
    Mail * mails;
    int mail_count;
    int mail_capacity;
} Post;

#endif
