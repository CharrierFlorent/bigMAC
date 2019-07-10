
/*PC8.c
 *Contient des fonctions permettant de filtrer les domaines d'un CSP
 *en utilisant l'algorithme PC8
 */

#include "PC8.h"
extern FILE * glb_output_file;
extern int consistancy;
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
int withoutsupport_PC(Constraint_mat * contraintes, int * domaine_k, int var_i, int var_j, int var_k, int a, int b, int size){
/*    if(!contraintes->constraint_matrix[var_i][var_k] || !contraintes->constraint_matrix[var_j][var_k])
        return false;
*/
    int taille_dom = size;
    if(!contraintes->constraint_matrix[var_i][var_k]){
        contraintes->constraint_matrix[var_i][var_k] = init_constraint(taille_dom);
        contraintes->constraint_matrix[var_k][var_i] = init_constraint(taille_dom);
        for(int l = 0; l < taille_dom; l++){
            for(int k = 0; k < taille_dom; k++){
                contraintes->constraint_matrix[var_i][var_k]->relations[l][k] = 1;
                contraintes->constraint_matrix[var_k][var_i]->relations[k][l] = 1;
            }
        }
    }

    if(!contraintes->constraint_matrix[var_j][var_k]){
        contraintes->constraint_matrix[var_j][var_k] = init_constraint(taille_dom);
        contraintes->constraint_matrix[var_k][var_j] = init_constraint(taille_dom);
        for(int l = 0; l < taille_dom; l++){
            for(int k = 0; k < taille_dom; k++){
                contraintes->constraint_matrix[var_j][var_k]->relations[l][k] = 1;
                contraintes->constraint_matrix[var_k][var_j]->relations[k][l] = 1;
            }
        }
    }



    int ** R_ik = contraintes->constraint_matrix[var_i][var_k]->relations;
    int ** R_jk = contraintes->constraint_matrix[var_j][var_k]->relations;
    int c = first(domaine_k, size); 
    while(c < last(domaine_k, size) && !((R_ik[a][c] == 1) && (R_jk[b][c] == 1 ))){ // rajouter le ! ?
        c = next_value(domaine_k, c, size);
    }
    return !((R_ik[a][c] == 1) && (R_jk[b][c] == 1));// rajouter le ! ?
}

/***
 * Etant donné deux variables contraintes entre elle i et j
 * verifie que chaque valeur du domaine de j possede un support dans i
 * sinon filtres ces valeurs du domaine de j. Met à jour la liste_PC
 * paramètre : - csp : un csp
 *             - list_PC : la liste des variables dont le domaine a déjà été filtré
 *             - status_PC : un tableau de booleen permettant d'éviter de dupliquer
 *               les variables filtrée présente dans list_PC
 *             - i : la variable i, un entier
 *             - j : la variable i, un entier
 * sortie : retourne la liste PC mis à jour
 ***/
List * check_support_PC(CSP * csp, List * list_PC, int dim1, int dim2, int dim3, int status_PC[][dim2][dim3], int i, int j, int a, int b){
    int *tuple_1 = malloc(3*sizeof(int));
    int *tuple_2 = malloc(3*sizeof(int));
    tuple_1[0] = i;
    tuple_1[1] = a;
    tuple_1[2] = j;
    tuple_2[0] = j;
    tuple_2[1] = b;
    tuple_2[2] = i;
    if(!(status_PC[i][a][j] != 1)){
        if(list_PC)
            list_PC = append(list_PC, tuple_1);
        else
            list_PC = append(NULL, tuple_1);
        status_PC[i][a][j] = 1;
    }
    else
        free(tuple_1);

    if(!(status_PC[j][b][i] != 1)){
        if(list_PC)
            list_PC = append(list_PC, tuple_2);
        else
            list_PC = append(NULL, tuple_2);
        status_PC[j][b][i] = 1;
    }
    else
        free(tuple_2);
    return list_PC;
}

/***
 * initialise list_PC, càd passe une premiere fois sur toute les contraintes
 * et vérifie les supports
 * paramètre : - csp : un csp
 *             - status_PC : un tableau de booleen permettant d'éviter de dupliquer
 *               les variables filtrée présente dans list_PC
 * sortie : Une liste list_PC
 ***/
List * initialize_PC8(CSP * csp,int profondeur, int dim1, int dim2, int dim3, int status_PC[][dim2][dim3]){
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
                            if(csp->Domain->domain_matrix[i][a] == 1 && csp->Domain->domain_matrix[j][b] == 1 && csp->matrice_contraintes->constraint_matrix[i][j] && csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] == 1){
                                consistancy++;
                                if(withoutsupport_PC(csp->matrice_contraintes, csp->Domain->domain_matrix[k],i,j,k,a,b,csp->Domain->max_domain)){
                                    csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] = -(profondeur);
                                    csp->matrice_contraintes->constraint_matrix[j][i]->relations[b][a] = -(profondeur);
                                    list_PC = check_support_PC(csp, list_PC, dim1, dim2, dim3,status_PC,i,j,a,b);
                                }
                            }
    }
    return list_PC;
}

/***
 * la propagation. Soit une variable i présente dans list_PC dont on a filtré le domaine
 * on va revirifier les supports de toute les variables en contraintes avec i.
 * paramètre : - csp : un csp
 *             - status_PC : un tableau de booleen permettant d'éviter de dupliquer
 *               les variables filtrée présente dans list_PC
 * sortie : retourne un pointeur sur la liste PC (màj au cours de l'execution de cette fonction)
 ***/
List * propagate_PC(CSP * csp, List * list_PC,int profondeur, int dim1, int dim2, int dim3, int status_PC[][dim2][dim3], int i, int k, int a){
    for(int j = 0; j < csp->max_var; j++)
        if( j != i && j != k )
            for(int b = 0; b < csp->Domain->max_domain; b++)
                if(csp->Domain->domain_matrix[i][a] == 1 && csp->Domain->domain_matrix[j][b] == 1 && csp->matrice_contraintes->constraint_matrix[i][j] && csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] == 1){
                    consistancy++;
                    if(withoutsupport_PC(csp->matrice_contraintes, csp->Domain->domain_matrix[k],i,j,k,a,b,csp->Domain->max_domain)){
                        csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] = -(profondeur);
                        csp->matrice_contraintes->constraint_matrix[j][i]->relations[b][a] = -(profondeur);
                        list_PC = check_support_PC(csp, list_PC,  dim1, dim2, dim3, status_PC,i,j,a,b);
                    }
                }
    return list_PC;
}

void check_PC8(CSP * csp){
    int nb_var = csp->max_var;


    for(int i = 0; i < nb_var; i++){
        for(int j = 0; j < nb_var; j++)
            for(int k = 0; k < nb_var;k++)
                if(i < j && k != i && k != j)
                    for(int a = 0; a < csp->Domain->max_domain; a++)
                        for(int b = 0; b < csp->Domain->max_domain; b++)
                            if(csp->Domain->domain_matrix[i][a] == 1 && csp->Domain->domain_matrix[j][b] == 1 && csp->matrice_contraintes->constraint_matrix[i][j] && csp->matrice_contraintes->constraint_matrix[i][j]->relations[a][b] == 1)
                                if(withoutsupport_PC(csp->matrice_contraintes, csp->Domain->domain_matrix[k],i,j,k,a,b,csp->Domain->max_domain)){
                                    printf("##error###\n");
                                }
    }
}


/***
 * Algorithme PC8, permet de maintenir la consistance
 * d'arc d'un csp. Filtre le domaine du csp.
 * paramètre : - csp : un CSP
 ***/
void PC8(CSP * csp, int profondeur){
    int a;
    int dim1 = csp->max_var, dim2 = csp->Domain->max_domain, dim3 = csp->max_var;
    int i,j,k;
    int status_PC[dim1][dim2][dim3];
    
    for (int x = 0; x< dim1; x++) {
        for (int y = 0; y < dim2; y++) {
            for(int z = 0; z < dim3; z++)
                status_PC[x][y][z] = 0;
        }
    }

    List * list_PC;
    list_PC = initialize_PC8(csp, profondeur, dim1, dim2, dim3, status_PC);
    
    int * data;
    while(list_PC != NULL){
        data = (int *)list_PC->value;
        i = data[0];
        a = data[1];
        k = data[2];
        list_PC = list_remove_first_and_data(list_PC);
        status_PC[i][a][k] = false;
        list_PC = propagate_PC(csp, list_PC, profondeur, dim1, dim2, dim3, status_PC,i, k, a);
    }
    list_destroy(list_PC);
    //check_PC8(csp);
}

