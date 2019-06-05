/*List.c
 *Contient toute les fonctions utiles à la création
 *et à la manipulation de listes chainées
 */

#include "list.h"

/***
 * Retourne le prochain maillon de la chaine
 * paramètre : - list : Une liste chainée
 * sortie : Un pointer List sur le prochain élément
 */

List * get_next(List *list){
    if(!list)
        return NULL;
    return list->next;
}

/***
 * Crée un maillon de la chaine
 * paramètre : - value : La valeur à mettre dans le maillon
 * sortie : Un pointer List sur l'élément crée
 */
List * create_list(void *value){
    List *list = malloc(sizeof(list)+3*sizeof(int));
    if (list){
        list->value = value;
        list->next = NULL;
    }
    return list;
}

/***
 * Ajoute un maillon au début d'une chaine existante
 * paramètre : - value : La valeur à mettre dans le nouveau maillon
 *             - old : Le 1er éléments d'une liste chainée
 * sortie : Un pointer sur la nouvelle liste
 */
List * prepend(List *old, void *value){
    List *list = create_list(value);
    if (list)
        list->next = old;
    return list;
}

/***
 * Ajoute un maillon en fin de la liste chainée.
 * Si la liste est vide, en crée une.
 * paramètre : - value : La valeur à mettre dans le nouveau maillon
 *             - list : La liste dans laquelle on veut ajouter un maillon
 * sortie : Un pointer sur la nouvelle liste
 */
List * append(List *list, void *value){
    List **plist = &list;
    while (*plist){
        plist = &(*plist)->next;
    }
    *plist = create_list(value);
    if (*plist)
        return list;
    else
        return NULL;
}

/***
 * Supprime le 1er maillon de la liste chainée.
 * paramètre : - list : Un pointeur sur une liste chainée
 * sortie : Un pointeur sur la nouvelle liste
 */
List * list_remove_first(List *list){
    if(!list)
        return NULL;
    List *first = list;
    list = list->next;
    free(first);
    return list;
}

/***
 * Supprime une liste chainée
 * paramètre : - list : Un pointeur sur une liste chainée
 */
void list_destroy(List *list){
    while (list)
        list = list_remove_first(list);
}

/***
 * Retourne la longueur d'une liste chainée
 * paramètre : - list : Un pointeur sur une liste chainée
 * sortie : la taille de la chaine
 */
int list_length(List *list){
    int length = 0;
    while (list){
        length++;
        list = list->next;
    }
    return length;
}
