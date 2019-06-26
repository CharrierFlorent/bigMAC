#ifndef H_FC
#define H_FC

#include "generateur.h"

int cherche_domaine_vide (int *taille_domaine, int max_var, int *status);
int choix_variable (CSP *probleme, HEURISTIQUE heuristique, int *status, int nb_var_instancie);
void reinitialise_domaine (CSP *probleme, int variable_courante, int valeur_courante, int *status);
void filtre_domaine (CSP *probleme, int variable_courante, int valeur_courante, int *status);
void Forward_Checking (CSP *probleme, int * solution,  HEURISTIQUE heuristique);
int FC(CSP * csp, int * solution, int * var_status, int profondeur);
void restore_domain(CSP * csp, int * var_status, int variable_courante, int profondeur);
int check_forward(CSP * csp, int * var_status, int variable_courante, int valeur_courant, int profondeur);
#endif
