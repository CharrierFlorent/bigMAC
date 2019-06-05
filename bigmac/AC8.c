/*AC8.c
 *Contient des fonctions permettant de filtrer les domaines d'un CSP
 *en utilisant l'algorithme AC8
 */

#include "AC8.h"

/***
 * Retourne la 1ere valeur du domaine d'une variable
 * W.I.P : pas encore certain de la structure de données
 * pour representer les variables, donc cette fonctions
 * pourrait un peu changer
 * paramètre : - domaine : le domaine d'une variable (attention,
 *               != de la structure Domain qui contient les domaines
 *               de toute les variables du probleme
 *             - size : Taille du domaine
 * sortie : retourne la valeur de la 1ere valeur du domaine disponible.
 *          Notons que dans notre representations, cette fonction retourne en fait un index
 *          car les tableaux domaines contiennent seulement les valeurs 0 (absent), 1 (présent) et -1 (filtrée)
 ***/
int first(int * domaine, int size){
    for(int i = 0; i < size; i++)
        if(domaine[i] == 1)
            return i;
    return 0;
}

/***
 * Retourne la derniere valeur du domaine d'une variable
 * Toute les remarques de la fonctions first sont aussi valide ici
 * paramètre : - domaine : le domaine d'une variable
 *             - size : Taille du domaine
 * sortie : retourne la valeur de la derniere valeur du domaine disponible.
 ***/
int last(int * domaine, int size){
    int a = 0;
    for(int i = 0; i < size; i++)
        if(domaine[i] == 1)
            a = i;
    return a;
}

/***
 * Retourne la valeur suivante du domaine d'une variable
 * Toute les remarques de la fonctions first sont aussi valide ici
 * paramètre : - domaine : le domaine d'une variable
 *             - i : valeur dont on cherche le successeur
 *             - size : Taille du domaine
 * sortie : retourne la valeur de la prochaine valeur du domaine disponible.
 *          Si cette valeur n'existe pas (i.e i est la derniere), retourne i.
 ***/
int next_value(int * domaine, int i, int size){
    for(int j = i+1; j < size; j++)
        if(domaine[j] == 1)
            return j;
    return i;
}

/***
 * Verifie si la valeur b du domaine d'une variable à un support
 * dans le domaine d'une autre variables
 * paramètre : - relations : la matrice de relations des variables i et j
 *             - domaine : le domaine de la variable i
 *             - var_i : la variable i, un entier
 *             - var_j : la variable j, un entier
 *             - b : Une valeur du domaine de j dont on cherche les supports
 *             - size : Taille du domaine
 * sortie : retourne Faux si b a un support dans i. Vrai sinon
 ***/
int withoutsupport_AC(int ** relations, int * domaine, int var_i, int var_j, int b, int size){
    int a = first(domaine, size);
    while(a < last(domaine, size) && !relations[a][b]){
        a = next_value(domaine, a, size);
    }
    return !relations[a][b];
}

/***
 * Etant donné deux variables contraintes entre elle i et j
 * verifie que chaque valeur du domaine de j possede un support dans i
 * sinon filtre ces valeurs du domaine de j
 * paramètre : - csp : un csp
 *             - list_ac : la liste des variables dont le domaine a déjà été filtré
 *             - status_AC : un tableau de booleen permettant d'éviter de dupliquer
 *               les variables filtrée présente dans list_ac
 *             - i : la variable i, un entier
 *             - j : la variable i, un entier
 * 			   - var_status : un tableau de booleen qui permet de savoir si une variable est déja instanciée,
 * 						  	  vaut NULL si pas de variables instanciées
 ***/
List * check_support(CSP * csp, List * list_ac, int * status_AC, int i, int j, int *var_status){
    int taille_domaine = csp->Domain->max_domain;
    for(int b = 0; b < taille_domaine; b++){
        if(csp->Domain->domain_matrix[j][b] != 1)
            continue;
        if(withoutsupport_AC(csp->matrice_contraintes->constraint_matrix[i][j]->relations, csp->Domain->domain_matrix[i], i,j,b,taille_domaine)){
			if (var_status == NULL || var_status[j] == 0)
			{
				csp->Domain->domain_matrix[j][b] = -1;
				csp->Domain->taille_domaine[j]--;
			}
            if(!status_AC[j]){
                if(!list_ac)
                    list_ac = append(NULL,&j);
                else
                    list_ac = append(list_ac,&j);
                status_AC[j] = 1;
            }
        }
    }
    return list_ac;
}

/***
 * initialise list_AC, càd passe une premiere fois sur toute les contraintes
 * et vérifie les supports
 * paramètre : - csp : un csp
 *             - status_AC : un tableau de booleen permettant d'éviter de dupliquer
 *               les variables filtrée présente dans list_ac
 * 			   - var_status : un tableau de booleen qui permet de savoir si une variable est déja instanciée,
 * 						  	  vaut NULL si pas de variables instanciées
 * sortie : Une liste list_AC
 ***/
List * initialize_AC8(CSP * csp, int * status_AC, int *var_status){
    int nb_var = csp->max_var;
    List * list_ac = NULL;
    for(int i = 0; i < nb_var; i++)
        status_AC[i] = 0;

    for(int i = 0; i < nb_var; i++){
        for(int j = 0; j < nb_var; j++){
                if(!csp->matrice_contraintes->constraint_matrix[i][j])
                    continue;
                list_ac = check_support(csp, list_ac, status_AC,i,j, var_status);
        }
    }
    return list_ac;
}

/***
 * la propagation. Soit une variable i présente dans list_AC dont on a filtré le domaine
 * on va revirifier les supports de toute les variables en contraintes avec i
 * paramètre : - csp : un csp
 *             - status_AC : un tableau de booleen permettant d'éviter de dupliquer
 *               les variables filtrée présente dans list_ac
 * 			   - var_status : un tableau de booleen qui permet de savoir si une variable est déja instanciée,
 * 						  	  vaut NULL si pas de variables instanciées
 ***/
void propagate_AC(CSP * csp, List * list_ac, int * status_AC, int i, int *var_status){
    for(int j = 0; j < csp->max_var; j++){
        if(!csp->matrice_contraintes->constraint_matrix[i][j])
            continue;
        list_ac = check_support(csp, list_ac, status_AC, i, j, var_status);
    }
//    return list_ac;
}

/***
 * Algorithme AC8, permet de maintenir la consistance
 * d'arc d'un csp. Filtre le domaine du csp.
 * paramètre : - csp : un CSP
 * 			   - var_status : un tableau de booleen qui permet de savoir si une variable est déja instanciée,
 * 						  	  vaut NULL si pas de variables instanciées
 ***/
void AC8(CSP * csp, int *var_status){
    int i;
    int * status_ac = malloc(csp->max_var*sizeof(int));
    List * list_ac = initialize_AC8(csp, status_ac, var_status);
    //list_ac = list_remove_first(list_ac);
    while(list_ac){
        i = *((int *)list_ac->value);
        list_ac = list_remove_first(list_ac);
        status_ac[i] = false;
        propagate_AC(csp, list_ac, status_ac,i, var_status);

    }
}
