/*List.c
 *Contient toute les fonctions utiles à la création
 *et à la manipulation de listes chainées
 */

#include "list_i.h"

/***
 * Retourne le prochain maillon de la chaine
 * paramètre : - list : Une liste chainée
 * sortie : Un pointer List sur le prochain élément
 */
List_i * i_get_next(List_i *list){
    if(!list)
        return NULL;
    return list->next;
}

/***
 * Crée un maillon de la chaine
 * paramètre : - value : La valeur à mettre dans le maillon
 * sortie : Un pointer List sur l'élément crée
 */
List_i * i_create_list(int value){
    List_i *list = malloc(sizeof(list)+3*sizeof(int));
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
List_i * i_prepend(List_i *old, int value){
    List_i *list = i_create_list(value);
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
List_i * i_append(List_i *list, int value){
    List_i **plist = &list;
    while (*plist){
        plist = &(*plist)->next;
    }
    *plist = i_create_list(value);
    if (*plist)
        return list;
    else
        return NULL;
}

/***
 * Supprime le 1er maillon de la liste chainée. Ne supprime
 * pas la mémoire possiblement alloué dans le void *
 * paramètre : - list : Un pointeur sur une liste chainée
 * sortie : Un pointeur sur la nouvelle liste
 */
List_i * i_list_remove_first(List_i *list){
    if(!list)
        return NULL;
    List_i *first = list;
    list = list->next;
    free(first);
    return list;
}

/***
 * Supprime une liste chainée
 * paramètre : - list : Un pointeur sur une liste chainée
 */
void i_list_destroy(List_i *list){
    while (list)
        list = i_list_remove_first(list);
}

/***
 * Retourne la longueur d'une liste chainée
 * paramètre : - list : Un pointeur sur une liste chainée
 * sortie : la taille de la chaine
 */
int i_list_length(List_i *list){
    int length = 0;
    while (list){
        length++;
        list = list->next;
    }
    return length;
}
