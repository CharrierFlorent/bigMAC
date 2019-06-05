/*List.c
 *Contient toute les fonctions utiles � la cr�ation
 *et � la manipulation de listes chain�es
 */

#include "list.h"

/***
 * Retourne le prochain maillon de la chaine
 * param�tre : - list : Une liste chain�e
 * sortie : Un pointer List sur le prochain �l�ment
 */

List * get_next(List *list){
    if(!list)
        return NULL;
    return list->next;
}

/***
 * Cr�e un maillon de la chaine
 * param�tre : - value : La valeur � mettre dans le maillon
 * sortie : Un pointer List sur l'�l�ment cr�e
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
 * Ajoute un maillon au d�but d'une chaine existante
 * param�tre : - value : La valeur � mettre dans le nouveau maillon
 *             - old : Le 1er �l�ments d'une liste chain�e
 * sortie : Un pointer sur la nouvelle liste
 */
List * prepend(List *old, void *value){
    List *list = create_list(value);
    if (list)
        list->next = old;
    return list;
}

/***
 * Ajoute un maillon en fin de la liste chain�e.
 * Si la liste est vide, en cr�e une.
 * param�tre : - value : La valeur � mettre dans le nouveau maillon
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
 * Supprime le 1er maillon de la liste chain�e.
 * param�tre : - list : Un pointeur sur une liste chain�e
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
 * Supprime une liste chain�e
 * param�tre : - list : Un pointeur sur une liste chain�e
 */
void list_destroy(List *list){
    while (list)
        list = list_remove_first(list);
}

/***
 * Retourne la longueur d'une liste chain�e
 * param�tre : - list : Un pointeur sur une liste chain�e
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
