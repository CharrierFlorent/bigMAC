#ifndef H_LIST
#define H_LIST
#include<stdio.h>
#include<stdlib.h>
typedef struct List List;

struct List{
    List *next;
    void *value;
};

List * get_next(List *list);
List * create_list(void *value);
List * prepend(List *old, void *value);
List * append(List *list, void *value);
List * list_remove_first(List *list);
void list_destroy(List *list);
int list_length(List *list);

#endif
