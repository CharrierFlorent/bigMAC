#ifndef H_STRUCTURE
#define H_STRUCTURE

#include<stdio.h>
#include<stdlib.h>
#include <string.h>
typedef enum {MIN_DOMAINE, DOMAINE_DEGRE} HEURISTIQUE;

typedef struct{
	int ** domain_matrix;
	int max_var;
	int max_domain;
	int * taille_domaine;
}DOMAIN;

typedef struct{
	int ** relations;
	int max_domain;
}Constraint;

typedef struct{
	Constraint *** constraint_matrix;
	int max_var;
}Constraint_mat;

typedef struct{
    DOMAIN * Domain;
    Constraint_mat * matrice_contraintes;
    int * liste_var;
    int max_var;
}CSP;

typedef struct{
	int ** instanciation_stack;
	int max_var;
}Stack;

DOMAIN * init_domain(int max_var, int max_domain);
Constraint_mat * init_constraint_mat(int max_var);
Constraint * init_constraint(int max_domain);
void init_stack(Stack * stack, int max_var);
void empty_matrix(int ** matrix, int nb_col, int nb_row);
void print_matrix(int ** matrix, int nb_col, int nb_row);
CSP * init_empty_csp(int max_domain, int max_var);
int verify(CSP * csp, int * solution);
void print_relation(CSP * csp);
void print_csp(CSP * csp);
void free_domain(DOMAIN * domain);
void free_constraint_mat(Constraint_mat * matrice_contrainte);
void free_constraint(Constraint * constraint);
void free_csp(CSP * csp);
void copy_domain(CSP * source, CSP * destination);
CSP * create_csp_by_copy(CSP * csp);

#endif // H_STRUCTURE

