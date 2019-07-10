#ifndef H_LISTI
#define H_LISTI
#include<stdio.h>
#include<stdlib.h>
typedef struct List_i List_i;

struct List_i{
    List_i *next;
    int value;
};

List_i * i_get_next(List_i *list);
List_i * i_create_list(int value);
List_i * i_prepend(List_i *old, int value);
List_i * i_append(List_i *list, int value);
List_i * i_list_remove_first(List_i *list);
void i_list_destroy(List_i *list);
int i_list_length(List_i *list);

#endif
