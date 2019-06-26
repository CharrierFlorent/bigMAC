/*structure.c
 *Contient toute les fonctions utiles à la création de CSP
 */

#include "structure.h"
 extern FILE * glb_output_file;
/*
  Contient toute les fonctions d'allocations et de libération
  mémoire des différentes structures
*/

/***
 * Crée un struct Domain, lui alloue de la mémoire
 * et initialise toute les valeurs à 1
 * paramètre : - max_var : le nombre de variables de notre CSP
 *             - max_domain : la taille du plus gros domaine du CSP
 * sortie : Un pointeur sur le nouveau struct Domain crée
 */
DOMAIN * init_domain(int max_var, int max_domain){
	DOMAIN * domain = malloc(sizeof(DOMAIN));
	domain->max_var = max_var;
	domain->max_domain = max_domain;
	domain->taille_domaine = malloc(max_var*(sizeof(domain->taille_domaine)));
	domain->domain_matrix = malloc(domain->max_var*(sizeof(domain->domain_matrix)));
	if(domain->domain_matrix == NULL){
		fprintf(stdout,"erreur allocation memoire domaine\n");
		exit(-1);
	}

	for(int i = 0; i < domain->max_var;i++){
		domain->domain_matrix[i] = malloc(domain->max_domain*(sizeof(domain->domain_matrix[i])));
		if(domain->domain_matrix == NULL){
			fprintf(stdout,"erreur allocation memoire domaine\n");
			exit(-1);
		}
	}

	for(int i =0; i < max_var; i++)
        for(int j = 0; j < max_domain; j++)
            domain->domain_matrix[i][j] = 1;

    for (int i=0; i<max_var; i++)
		domain->taille_domaine[i] = max_domain;
	return domain;
}

/***
 * Crée un struct Constraint_mat, lui alloue de la mémoire
 * la matrice de contrainte crée est peuplé de pointeur NULL
 * paramètre : - max_var : le nombre de variables de notre CSP
 * sortie : Un pointeur sur le nouveau struct Constraint_mat crée
 */
Constraint_mat * init_constraint_mat(int max_var){
    Constraint_mat * matrice_contrainte = malloc(sizeof(Constraint_mat));
	matrice_contrainte->max_var = max_var;
	matrice_contrainte->constraint_matrix = malloc(max_var*sizeof(struct Constraint *));
	if(matrice_contrainte->constraint_matrix == NULL){
		fprintf(stdout,"Erreur allocation memoire matrice de contrainte\n");
		exit(-1);
	}
	for(int i = 0; i < max_var; i++){
		matrice_contrainte->constraint_matrix[i] = malloc(max_var*sizeof(Constraint));
		if(matrice_contrainte->constraint_matrix == NULL){
			fprintf(stdout,"erreur allocation memoire matrice de domaine\n");
			exit(-1);
		}
	}
	return matrice_contrainte;
}

/***
 * Crée un struct Constraint, lui alloue de la mémoire
 * la matrice de relations de la Constraint est initialisé à 0
 * paramètre : - max_domain : la taille du plus gros domaine du CSP
 * sortie : Un pointeur sur le nouveau struct Constraint_mat crée
 */
Constraint * init_constraint(int max_domain){
	Constraint * constraint = malloc(sizeof(Constraint));
	constraint->max_domain = max_domain;
	constraint->relations = malloc(max_domain*sizeof(constraint->relations));
	if(constraint->relations == NULL){
		fprintf(stdout,"Erreur allocation memoire contrainte\n");
		exit(-1);
	}
	for(int i = 0; i < max_domain; i++){
		constraint->relations[i] = malloc(max_domain*sizeof(constraint->relations[i]));
		if(constraint->relations[i] == NULL){
			fprintf(stdout,"erreur allocation memoire contrainte \n");
			exit(-1);
		}
	}

	for(int i = 0; i < max_domain; i++){
        for(int j = 0; j < max_domain; j++)
            constraint->relations[i][j] = 0;
	}
	return constraint;
}

void init_stack(Stack * stack, int max_var){
	stack->instanciation_stack = malloc(max_var*sizeof(stack->instanciation_stack));
	if(stack->instanciation_stack == NULL){
		fprintf(stdout,"erreur allocation memoire pile\n");
		exit(-1);
	}
	for(int i = 0 ; i < max_var; i++){
		stack->instanciation_stack[i] = malloc(2*sizeof(stack->instanciation_stack[i]));
		if(stack->instanciation_stack[i] == NULL){
			fprintf(stdout,"erreur allocation memoire pile \n");
			exit(-1);
		}
	}
}

/***
 * Remplit une matrice de 0
 * paramètre : - matrix : une matrice d'entier
 *             - nb_col : le nombre de colonne de la matrice
 *             - nb_row : le nombre de ligne de la matrice
 */
void empty_matrix(int ** matrix, int nb_col, int nb_row){
	for(int i = 0 ; i < nb_col; i++)
		for(int j = 0; j < nb_row; j++)
			matrix[i][j] = 0;
}

/***
 * Affiche le contenu d'une matrice sur la sortie standard
 * paramètre : - matrix : une matrice d'entier
 *             - nb_col : le nombre de colonne de la matrice
 *             - nb_row : le nombre de ligne de la matrice
 */
void print_matrix(int ** matrix, int nb_col, int nb_row){
	for(int i = 0 ; i < nb_col; i++){
		for(int j = 0; j < nb_row; j++){
			fprintf(glb_output_file," %d ", matrix[i][j]);
		}
		fprintf(glb_output_file,"\n");
	}
}

void print_domaine(CSP * csp){
	print_matrix(csp->Domain->domain_matrix, csp->max_var, csp->Domain->max_domain);
}

void print_relation(CSP * csp){
    for(int i =0; i < csp->max_var;i++){
        for(int j = 0; j < csp->max_var; j++){
            if(csp->matrice_contraintes->constraint_matrix[i][j]){
                fprintf(glb_output_file,"%d et %d\n",i,j);
                print_matrix(csp->matrice_contraintes->constraint_matrix[i][j]->relations, csp->Domain->max_domain, csp->Domain->max_domain);
            }
        }
    }
}

void print_csp(CSP * csp){
        fprintf(glb_output_file,"\n");
        fprintf(glb_output_file,"Domaine : \n");
        print_matrix(csp->Domain->domain_matrix, csp->max_var, csp->Domain->max_domain);
        fprintf(glb_output_file,"\nrelations : \n");
        print_relation(csp);
        fprintf(glb_output_file,"\n");
}

/***
 * Permet de tester si une solution est valide.
 * paramètre : - csp : un csp
 *             - solution : un tableau contenant une solutions au csp que l'on veut tester
 ***/
int verify(CSP * csp, int * solution){
    for(int i = 0; i < csp->max_var; i++)
        for(int j = 0; j < csp->max_var; j++){
                if(csp->matrice_contraintes->constraint_matrix[i][j]){
                    if(!csp->matrice_contraintes->constraint_matrix[i][j]->relations[solution[i]][solution[j]])
                        return 0;
                }
        }
    return 1;
}

/***
 * Crée un CSP vide
 * paramètre : - max_var : le nombre de variables de notre CSP
 *             - max_domain : la taille du plus gros domaine du CSP
 * sortie : Un pointeur sur le nouveau CSP crée
 */
CSP * init_empty_csp(int max_var, int max_domain){
	CSP * csp;
	csp = malloc(sizeof(CSP));
	csp->max_var = max_var;

	csp->Domain = init_domain(max_var, max_domain);
	csp->matrice_contraintes = init_constraint_mat(max_var);
	for(int i = 0; i < max_var; i++)
        for(int j = 0; j < max_var; j++)
            csp->matrice_contraintes->constraint_matrix[i][j] = NULL;
	return csp;
}

/***
 * libère la mémoire d'un struct Domain
 * paramètre : - domain : un struct Domain
 */
void free_domain(DOMAIN * domain){
	for(int i = 0; i < domain->max_var;i++){
		free(domain->domain_matrix[i]);
	}
	free(domain->domain_matrix);
	free(domain->taille_domaine);
	free(domain);
}

/***
 * libère la mémoire d'un struct Constraint_mat
 * paramètre : - matrice_contrainte : un struct Constraint_mat
 */
void free_constraint_mat(Constraint_mat * matrice_contrainte){
	for(int i = 0; i < matrice_contrainte->max_var; i++){
        free(matrice_contrainte->constraint_matrix[i]);
	}
    free(matrice_contrainte->constraint_matrix);
    free(matrice_contrainte);
}

/***
 * libère la mémoire d'un struct Constraint
 * paramètre : - constraint : un struct Constraint
 */
void free_constraint(Constraint * constraint){
	for(int i = 0; i < constraint->max_domain; i++){
		free(constraint->relations[i]);
	}
    free(constraint->relations);
	free(constraint);
}

/***
 * libère la mémoire d'un struct CSP
 * paramètre : - csp : un struct CSP
 */
void free_csp(CSP * csp){

	for(int i = 0; i < csp->max_var;i++)
		for(int j = 0; j < csp->max_var;j++){
			for(int k = 0; k < csp->Domain->max_domain; k++)
				if(csp->matrice_contraintes->constraint_matrix[i][j])
					free(csp->matrice_contraintes->constraint_matrix[i][j]->relations[k]);
			if(csp->matrice_contraintes->constraint_matrix[i][j])
				free(csp->matrice_contraintes->constraint_matrix[i][j]->relations);
		}	



	for(int i = 0; i < csp->max_var;i++){
		for(int j = 0; j < csp->max_var;j++)
			free(csp->matrice_contraintes->constraint_matrix[i][j]);
	}
    free_domain(csp->Domain);
    free_constraint_mat(csp->matrice_contraintes);
    //free(csp->liste_var);
    free(csp);
}

void copy_domain(CSP * source, CSP * destination){
    for(int i = 0; i < source->Domain->max_var; i++)
        for(int j = 0; j < source->Domain->max_domain; j++)
            destination->Domain->domain_matrix[i][j] = source->Domain->domain_matrix[i][j];
}



CSP * create_csp_by_copy(CSP * csp){
	CSP * copy;
	int domain_size = csp->Domain->max_domain;
	int max_var = csp->max_var;
	copy = malloc(sizeof(CSP));
	copy->max_var = csp->max_var;
	copy->Domain = init_domain(csp->max_var, domain_size);
	copy->matrice_contraintes = init_constraint_mat(csp->max_var);
	for(int i = 0; i < max_var; i++)
        for(int j = 0; j < max_var; j++)
            if(csp->matrice_contraintes->constraint_matrix[i][j]){
                copy->matrice_contraintes->constraint_matrix[i][j] = init_constraint(domain_size);
                copy->matrice_contraintes->constraint_matrix[i][j]->max_domain = domain_size;
                for(int k = 0; k < domain_size;k++)
                    for(int t = 0; t < domain_size; t++)
                        copy->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[k][t];
                }
            else
                copy->matrice_contraintes->constraint_matrix[i][j] = NULL;

    copy_domain(csp, copy);

	return copy;
}
