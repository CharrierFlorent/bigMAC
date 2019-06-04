/*structure.c
 *Contient toute les fonctions utiles � la cr�ation de CSP
 */

#include "structure.h"
/*
  Contient toute les fonctions d'allocations et de lib�ration
  m�moire des diff�rentes structures
*/

/***
 * Cr�e un struct Domain, lui alloue de la m�moire
 * et initialise toute les valeurs � 1
 * param�tre : - max_var : le nombre de variables de notre CSP
 *             - max_domain : la taille du plus gros domaine du CSP
 * sortie : Un pointeur sur le nouveau struct Domain cr�e
 */
DOMAIN * init_domain(int max_var, int max_domain){
	DOMAIN * domain = malloc(sizeof(DOMAIN));
	domain->max_var = max_var;
	domain->max_domain = max_domain;
	domain->taille_domaine = malloc(max_var*(sizeof(domain->taille_domaine)));
	domain->domain_matrix = malloc(domain->max_var*(sizeof(domain->domain_matrix)));
	if(domain->domain_matrix == NULL){
		printf("erreur allocation memoire domaine\n");
		exit(-1);
	}

	for(int i = 0; i < domain->max_var;i++){
		domain->domain_matrix[i] = malloc(domain->max_domain*(sizeof(domain->domain_matrix[i])));
		if(domain->domain_matrix == NULL){
			printf("erreur allocation memoire domaine\n");
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
 * Cr�e un struct Constraint_mat, lui alloue de la m�moire
 * la matrice de contrainte cr�e est peupl� de pointeur NULL
 * param�tre : - max_var : le nombre de variables de notre CSP
 * sortie : Un pointeur sur le nouveau struct Constraint_mat cr�e
 */
Constraint_mat * init_constraint_mat(int max_var){
    Constraint_mat * matrice_contrainte = malloc(sizeof(Constraint_mat));
	matrice_contrainte->max_var = max_var;
	matrice_contrainte->constraint_matrix = malloc(max_var*sizeof(struct Constraint *));
	if(matrice_contrainte->constraint_matrix == NULL){
		printf("Erreur allocation memoire matrice de contrainte\n");
		exit(-1);
	}
	for(int i = 0; i < max_var; i++){
		matrice_contrainte->constraint_matrix[i] = malloc(max_var*sizeof(Constraint));
		if(matrice_contrainte->constraint_matrix == NULL){
			printf("erreur allocation memoire matrice de domaine\n");
			exit(-1);
		}
	}
	return matrice_contrainte;
}

/***
 * Cr�e un struct Constraint, lui alloue de la m�moire
 * la matrice de relations de la Constraint est initialis� � 0
 * param�tre : - max_domain : la taille du plus gros domaine du CSP
 * sortie : Un pointeur sur le nouveau struct Constraint_mat cr�e
 */
Constraint * init_constraint(int max_domain){
	Constraint * constraint = malloc(sizeof(Constraint));
	constraint->max_domain = max_domain;
	constraint->relations = malloc(max_domain*sizeof(constraint->relations));
	if(constraint->relations == NULL){
		printf("Erreur allocation memoire contrainte\n");
		exit(-1);
	}
	for(int i = 0; i < max_domain; i++){
		constraint->relations[i] = malloc(max_domain*sizeof(constraint->relations[i]));
		if(constraint->relations[i] == NULL){
			printf("erreur allocation memoire contrainte \n");
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
		printf("erreur allocation memoire pile\n");
		exit(-1);
	}
	for(int i = 0 ; i < max_var; i++){
		stack->instanciation_stack[i] = malloc(2*sizeof(stack->instanciation_stack[i]));
		if(stack->instanciation_stack[i] == NULL){
			printf("erreur allocation memoire pile \n");
			exit(-1);
		}
	}
}

/***
 * Remplit une matrice de 0
 * param�tre : - matrix : une matrice d'entier
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
 * param�tre : - matrix : une matrice d'entier
 *             - nb_col : le nombre de colonne de la matrice
 *             - nb_row : le nombre de ligne de la matrice
 */
void print_matrix(int ** matrix, int nb_col, int nb_row){
	for(int i = 0 ; i < nb_col; i++){
		for(int j = 0; j < nb_row; j++){
			printf(" %d ", matrix[i][j]);
		}
		printf("\n");
	}
}

void print_relation(CSP * csp){
    for(int i =0; i < csp->max_var;i++){
        for(int j = 0; j < csp->max_var; j++){
            if(csp->constraint_matrix[i][j]){
                printf("%d et %d\n",i,j);
                print_matrix(csp->constraint_matrix[i][j]->relations, csp->max_domain, csp->max_domain);
            }
        }
    }
}

/***
 * Cr�e un CSP vide
 * param�tre : - max_var : le nombre de variables de notre CSP
 *             - max_domain : la taille du plus gros domaine du CSP
 * sortie : Un pointeur sur le nouveau CSP cr�e
 */
CSP * init_empty_csp(int max_var, int max_domain){
	CSP * csp;
	csp = malloc(sizeof(CSP));
	csp->taille_domaine = malloc(max_var*sizeof(int));
	csp->max_var = max_var;
	csp->max_domain = max_domain;
	csp->domain_matrix = malloc(csp->max_var*(sizeof(csp->domain_matrix)));
	if(csp->domain_matrix == NULL){
		printf("erreur allocation memoire domaine\n");
		exit(-1);
	}

	for(int i = 0; i < csp->max_var;i++){
		csp->domain_matrix[i] = malloc(csp->max_domain*(sizeof(csp->domain_matrix[i])));
		if(csp->domain_matrix == NULL){
			printf("erreur allocation memoire domaine\n");
			exit(-1);
		}
	}


	for(int i =0; i < max_var; i++)
        for(int j = 0; j < max_domain; j++)
            csp->domain_matrix[i][j] = 1;

    for (int i=0; i<max_var; i++)
		csp->taille_domaine[i] = max_domain;


	csp->constraint_matrix = malloc(max_var*sizeof(struct Constraint *));
	if(csp->constraint_matrix == NULL){
		printf("Erreur allocation memoire matrice de contrainte\n");
		exit(-1);
	}
	for(int i = 0; i < max_var; i++){
		csp->constraint_matrix[i] = malloc(max_var*sizeof(Constraint));
		if(csp->constraint_matrix == NULL){
			printf("erreur allocation memoire matrice de domaine\n");
			exit(-1);
		}
	}

	for(int i = 0; i < max_var; i++)
        for(int j = 0; j < max_var; j++)
            csp->constraint_matrix[i][j] = NULL;
	return csp;
}

/***
 * lib�re la m�moire d'un struct Domain
 * param�tre : - domain : un struct Domain
 */
void free_domain(DOMAIN * domain){
	for(int i = 0; i < domain->max_var;i++){
		free(domain->domain_matrix[i]);
	}
	free(domain->domain_matrix);
	free(domain);
}

/***
 * lib�re la m�moire d'un struct Constraint_mat
 * param�tre : - matrice_contrainte : un struct Constraint_mat
 */
void free_constraint_mat(Constraint_mat * matrice_contrainte){
	for(int i = 0; i < matrice_contrainte->max_var; i++){
        free(matrice_contrainte->constraint_matrix[i]);
	}
    free(matrice_contrainte->constraint_matrix);
    free(matrice_contrainte);
}

/***
 * lib�re la m�moire d'un struct Constraint
 * param�tre : - constraint : un struct Constraint
 */
void free_constraint(Constraint * constraint){
	for(int i = 0; i < constraint->max_domain; i++){
		free(constraint->relations[i]);
	}
    free(constraint->relations);
	free(constraint);
}

/***
 * lib�re la m�moire d'un struct CSP
 * param�tre : - csp : un struct CSP
 */
void free_csp(CSP * csp){
	free(csp->taille_domaine);
	for(int i = 0; i < csp->max_var; i++){
		for(int j = 0; j < csp->max_var; j++)
        	if(csp->constraint_matrix[i][j]){
				free(csp->constraint_matrix[i][j]->relations);
        	}
	}
    
    for(int i = 0; i < csp->max_var; i++){
        free(csp->constraint_matrix[i]);
	}
    free(csp->constraint_matrix);
    free(csp);
}

/*
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
*/