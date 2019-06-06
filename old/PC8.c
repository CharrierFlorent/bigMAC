
/*PC8.c
 *Contient des fonctions permettant de filtrer les domaines d'un CSP
 *en utilisant l'algorithme PC8
 */

#include "PC8.h"


/***
 * Verifie si la valeur b du domaine d'une variable � un support
 * dans le domaine d'une autre variables
 * param�tre : - relations : la matrice de relations des variables i et j
 *             - domaine : le domaine de la variable i
 *             - var_i : la variable i, un entier
 *             - var_j : la variable j, un entier
 *             - b : Une valeur du domaine de j dont on cherche les supports
 *             - size : Taille du domaine
 * sortie : retourne Faux si b a un support dans i. Vrai sinon
 ***/
int withoutsupport_PC(Constraint_mat * contraintes, int * domaine_k, int var_i, int var_j, int var_k, int a, int b, int size){
    if(!contraintes->constraint_matrix[var_i][var_k] || !contraintes->constraint_matrix[var_j][var_k])
        return false;

    int ** R_ik = contraintes->constraint_matrix[var_i][var_k]->relations;
    int ** R_jk = contraintes->constraint_matrix[var_j][var_k]->relations;
    int c = first(domaine_k, size);
    while(c < last(domaine_k, size) && !(R_ik[a][c] && R_jk[b][c])){
        c = next_value(domaine_k, c, size);
    }
    return !(R_ik[a][c] && R_jk[b][c]);
}

/***
 * Etant donn� deux variables contraintes entre elle i et j
 * verifie que chaque valeur du domaine de j possede un support dans i
 * sinon filtres ces valeurs du domaine de j. Met � jour la liste_PC
 * param�tre : - csp : un csp
 *             - list_PC : la liste des variables dont le domaine a d�j� �t� filtr�
 *             - status_PC : un tableau de booleen permettant d'�viter de dupliquer
 *               les variables filtr�e pr�sente dans list_PC
 *             - i : la variable i, un entier
 *             - j : la variable i, un entier
 * sortie : retourne la liste PC mis � jour
 ***/
List * check_support_PC(CSP * csp, List * list_PC, int *** status_PC, int i, int j, int a, int b){
    int *tuple_1 = malloc(3*sizeof(int));
    int *tuple_2 = malloc(3*sizeof(int));
    tuple_1[0] = i;
    tuple_1[1] = a;
    tuple_1[2] = j;
    tuple_2[0] = j;
    tuple_2[1] = b;
    tuple_2[2] = i;
    if(!status_PC[i][a][j]){
        if(list_PC)
            list_PC = append(list_PC, tuple_1);
        else
            list_PC = append(NULL, tuple_1);
        status_PC[i][a][j] = true;
    }
    if(!status_PC[j][b][i]){
        if(list_PC)
            list_PC = append(list_PC, tuple_2);
        else
            list_PC = append(NULL, tuple_2);
        status_PC[j][b][i] = true;
    }
    return list_PC;
}

/***
 * initialise list_PC, c�d passe une premiere fois sur toute les contraintes
 * et v�rifie les supports
 * param�tre : - csp : un csp
 *             - status_PC : un tableau de booleen permettant d'�viter de dupliquer
 *               les variables filtr�e pr�sente dans list_PC
 * sortie : Une liste list_PC
 ***/
List * initialize_PC8(CSP * csp, int *** status_PC){
    int nb_var = csp->max_var;
    List * list_PC = NULL;
    for(int i = 0; i < nb_var; i++)
        for(int j = 0; j < nb_var; j++)
            if(i != j)
                for(int a = 0; a < csp->Domain->max_domain; a++)
                    if(csp->Domain->domain_matrix[i][a] == 1)
                        status_PC[i][a][j] = 0;

    for(int i = 0; i < nb_var; i++){
        for(int j = 0; j < nb_var; j++)
            for(int k = 0; k < nb_var;k++)
                if(i < j && k != i && k != j)
                    for(int a = 0; a < csp->Domain->max_domain; a++)
                        for(int b = 0; b < csp->Domain->max_domain; b++)
                            if(csp->matrice_contraintes->constraint_matrix[i][j] && csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b])
                                if(withoutsupport_PC(csp->matrice_contraintes, csp->Domain->domain_matrix[k],i,j,k,a,b,csp->Domain->max_domain)){
                                    csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] = 0;
                                    csp->matrice_contraintes->constraint_matrix[j][i]->relations[b][a] = 0;
                                    list_PC = check_support_PC(csp, list_PC, status_PC,i,j,a,b);

                                }
    }
    return list_PC;
}

/***
 * la propagation. Soit une variable i pr�sente dans list_PC dont on a filtr� le domaine
 * on va revirifier les supports de toute les variables en contraintes avec i.
 * param�tre : - csp : un csp
 *             - status_PC : un tableau de booleen permettant d'�viter de dupliquer
 *               les variables filtr�e pr�sente dans list_PC
 * sortie : retourne un pointeur sur la liste PC (m�j au cours de l'execution de cette fonction)
 ***/
List * propagate_PC(CSP * csp, List * list_PC, int *** status_PC, int i, int k, int a){
    for(int j = 0; j < csp->max_var; j++)
        if( j != i && j != k )
            for(int b = 0; b < csp->Domain->max_domain; b++)
                if(csp->Domain->domain_matrix[j][b] && csp->matrice_contraintes->constraint_matrix[i][j] && csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b])
                    if(withoutsupport_PC(csp->matrice_contraintes, csp->Domain->domain_matrix[k],i,j,k,a,b,csp->Domain->max_domain)){
                        csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] = 0;
                        csp->matrice_contraintes->constraint_matrix[j][i]->relations[b][a] = 0;
                        list_PC = check_support_PC(csp, list_PC, status_PC,i,j,a,b);
                    }

    return list_PC;
}

/***
 * Algorithme PC8, permet de maintenir la consistance
 * d'arc d'un csp. Filtre le domaine du csp.
 * param�tre : - csp : un CSP
 ***/
void PC8(CSP * csp){
    int a;
    int dim1 = csp->max_var, dim2 = csp->Domain->max_domain, dim3 = csp->max_var;
    int i,j,k;
    int *** status_PC = (int ***)malloc(dim1*sizeof(int**));
    for (i = 0; i< dim1; i++) {
        status_PC[i] = (int **) malloc(dim2*sizeof(int *));
        for (j = 0; j < dim2; j++) {
            status_PC[i][j] = (int *)malloc(dim3*sizeof(int));
        }
    }
    List * list_PC;
    list_PC = initialize_PC8(csp, status_PC);
    int * data;
    while(list_PC != NULL){
        data =  (int *)list_PC->value;
        i = data[0];
        a = data[1];
        k = data[2];
        list_PC = list_remove_first(list_PC);
        status_PC[i][a][k] = false;
        list_PC = propagate_PC(csp, list_PC, status_PC,i, k, a);
    }
}
