#ifndef H_RFL
#define H_RFL

#include "generateur.h"
#include "FC.h"
#define EXPLORED -3          //Valeurs deja explorer à un niveau k, doivent être restauré seulement quand on remonte au niveau k-1
#define NOT_AFFECTED -2      //Quand on affecte une valeur, on met toute les autres en NOT_AFFECTED
#define OFFSET 10			 //OFFSET est utilisé pour choisir la valeur representant le filtrage. Comme on reserve certaines valeurs tel que -3 et -2
 							 //pour EXPLORED et NOT_AFFECTED, et qu'on veut garder en mémoire quand et quelles valeurs ont été filtrées, on rajoute
 							 //offset à la valeur de filtrage, cela nous permet ainsi de les distinguer des valeur particuliere tel que EXPLORED

void remove_explored_value(CSP * probleme, int variable_courante, int valeur_courante);
void reload_domain(CSP * probleme, int variable_courante, int valeur_courante, int profondeur);
void reduce_domain(CSP * probleme, int variable_courante, int valeur_courante);
int RFL (CSP *probleme, int * sol, HEURISTIQUE heuristique, struct timeval * st, struct timeval * et);

#endif
