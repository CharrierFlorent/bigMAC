/*RFL.c
 *Contient des fonction destiné à la résolution des CSP
 *en utilisant l'algorithme du Real Full Look-ahead (RFL)
 */
#include "RFL.h"
#include "AC8.h"

extern FILE * glb_output_file;
int noeud_RFL = 0;

/***
 * Fonction utilisé par RFL. Elle met à jour le domaine d'une variable
 * lorsque l'on backtrack dans RFL. C'est à dire que si l'affectation d'une
 * variable i+1 echoue, on backtrack sur la variable i, et on doit supprimé la
 * valeur qui lui était actuellement affecté pour ne pas la reaffecter ensuite.
 * Ces valeurs restent dans l'etat EXPLORED jusqu'a qu'on backtrack au niveau au dessus
 * Dans ce cas, c'est RFL qui se charge de restaurer ces valeurs
 * Paramètre : - probleme correspond au probleme CSP à résoudre
 * 			   - variable_courante est la variable etudié
 *			   - valeur_courante est la valeur choisis pour la variable courante
 */
void remove_explored_value(CSP * probleme, int variable_courante, int valeur_courante){
	probleme->Domain->domain_matrix[variable_courante][valeur_courante] = EXPLORED;
	int cpt = 0;
	for (int j=0; j<probleme->Domain->max_domain; j++){
		if(probleme->Domain->domain_matrix[variable_courante][j] == EXPLORED ||probleme->Domain->domain_matrix[variable_courante][j] == NOT_AFFECTED || probleme->Domain->domain_matrix[variable_courante][j] < 0)
			cpt++;
	}
	probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain - cpt;

}

/***
 * Fonction utilisé par RFL. A chaque affectation essayé, on va maintenir la
 * consistance d'arc et l'on réduit le domaine de la variable uniquement à la variable affecté
 * Dans le cas ou l'affectation echoue, il faut donc restorer les valeurs des domaines
 * potentiellement supprimé par AC8, et celle non affecté, dans l'etat NOT_AFFECTED
 * On utilise la profondeur dans AC8 pour marqué les valeur filtré à chaque niveau de 
 * parcours de l'arbre. Ainsi, quand on remonte, on sait quelles sont les valeurs restauré
 * On utilise un offset pour pouvoir se reserver des valeurs spécifique de filtrage 
 * Paramètre : - probleme correspond au probleme CSP à résoudre
 * 			   - variable_courante est la variable etudié
 *			   - valeur_courante est la valeur choisis pour la variable courante
 *			   - profondeur est la profondeur courante du parcours de l'arbre de recherche
 */
void reload_domain(CSP * probleme, int variable_courante, int valeur_courante, int profondeur){
	for(int i = 0; i < probleme->max_var; i++)
		for(int d = 0; d < probleme->Domain->max_domain; d++){
			if(i == variable_courante && (probleme->Domain->domain_matrix[variable_courante][d] == NOT_AFFECTED)){
	    		probleme->Domain->domain_matrix[variable_courante][d] = 1;
	    		probleme->Domain->taille_domaine[variable_courante]++;

			}
	    	if(probleme->Domain->domain_matrix[i][d] == -(OFFSET+profondeur)){
	    		probleme->Domain->domain_matrix[i][d] = 1;
	    		probleme->Domain->taille_domaine[i]++;
	    	}

		}

}

void reduce_domain(CSP * probleme, int variable_courante, int valeur_courante){
	
    for(int d = 0; d < probleme->Domain->max_domain; d++)
        if(d != valeur_courante && probleme->Domain->domain_matrix[variable_courante][d] == 1){
            probleme->Domain->domain_matrix[variable_courante][d] = NOT_AFFECTED;
            probleme->Domain->taille_domaine[variable_courante]--;
        }

}

/***
 * Affiche la solution au problème passé en paramètre en utilisant l'algorithme de RFL
 * remarque : utilise les fonctions choix_variables et cherche_domaine_vide du fichier FC.c
 * Paramètre : - probleme correspond au probleme CSP à résoudre
 * 			   - heuristique est l'heuritisque utilisé pour le choix des variables
 * sortie : génère la solution au probleme sur la sortie standart
 */
void RFL (CSP *probleme , int * sol,  HEURISTIQUE heuristique)
{
	fprintf(glb_output_file,"**************RFL**************\n");
	int variable_courante, valeur_courante;
	int affect = 0;			// est égal à 1 si on a réussit à trouver une affectation de la variable courante, 0 sinon
	int *var_status = malloc(sizeof(int) * probleme->max_var);	// var_status[0] = 1 signifie que la 1er variable est déja instancié, 0 sinon)
	int *tab_order_var = malloc(sizeof(int) * probleme->max_var);	// tableau représentant dans l'ordre les variables instanciées (ex : tab_order_var[0] = 5 signifie que la variable 5 est la 1er variable instancié)

	// Initialisation
	for (int i=0; i<probleme->max_var; i++){
		var_status[i] = 0;
		tab_order_var[i] = -1;
	}
	int nb_var_instancie = 0;	// correspond aux nombres de variables instanciées par RFL
    variable_courante = 0;// choix_variable(probleme, heuristique, var_status, nb_var_instancie);
    tab_order_var[nb_var_instancie] = variable_courante;

	while (nb_var_instancie < probleme->max_var){
		var_status[variable_courante] = 1;
		affect = 0;
		//on instancie la variable courante
		for (valeur_courante = 0; valeur_courante < probleme->Domain->max_domain; valeur_courante++){
				if (probleme->Domain->domain_matrix[variable_courante][valeur_courante] == 1){	
					noeud_RFL++;			
					reduce_domain(probleme, variable_courante, valeur_courante);
					// on applique un algorithme de consistance d'arc
					AC8(probleme, OFFSET+nb_var_instancie, NULL); //le 2eme argument sert à retrouver quels sont les valeur filtrées, je rajoute 
																	//le +10 pour me reserver des valeurs speciale au cas ou
					
					// si aucun domaine n'est vide on affecte cette valeur à la variable courante, sinon on passe à la prochaine valeur
					if (cherche_domaine_vide(probleme->Domain->taille_domaine, probleme->Domain->max_var, var_status) == 0){
						affect = 1;
						break;
					}
					else{
						reload_domain(probleme, variable_courante, valeur_courante, nb_var_instancie);
					}
				}
		}

		if (affect == 0){
			// si on revient au début et qu'on a pas réussi à affecter la variable alors pas de solution
			if (variable_courante == tab_order_var[0])
            {
            	fprintf(glb_output_file,"tab order %d\n", tab_order_var[0]);
				free(var_status);
				free(tab_order_var);
                fprintf(glb_output_file,"pas de solution\n");
                return;
            }
			// on réinitialise les valeurs deja exploré de la variable courante
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == EXPLORED){
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
					probleme->Domain->taille_domaine[variable_courante]++;
				}

			reload_domain(probleme, variable_courante, valeur_courante, nb_var_instancie);

			// "retour arrière"
			var_status[variable_courante] = 0;
			nb_var_instancie--;
			variable_courante = tab_order_var[nb_var_instancie];
			// on réinitialise le domaine de la variable précédente et on enleve du domaine la précédente valeur inconsistante
			reload_domain(probleme, variable_courante, sol[variable_courante], nb_var_instancie);
			remove_explored_value(probleme, variable_courante, sol[variable_courante]);
		}
		else{
			sol[variable_courante] = valeur_courante; 
			// si on a réussit à affecter une valeur à la variable alors on supprime toute les autres valeurs du domaine de la variable
			/*for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == 1 && j != valeur_courante)
					probleme->Domain->domain_matrix[variable_courante][j] = NOT_AFFECTED;
			probleme->Domain->taille_domaine[variable_courante] = 1;*/
			// on choisit la prochaine variable
			tab_order_var[nb_var_instancie] = variable_courante;
			variable_courante = choix_variable(probleme, heuristique, var_status, nb_var_instancie); 
			if(variable_courante == -1){
				
			}
			nb_var_instancie++;
		}
	}
	
	// affichage de la solution
	fprintf(glb_output_file,"solution : \n");
	for(int i=0; i < probleme->max_var; i++)
		fprintf(glb_output_file,"x%d = %d \n",i,sol[i]);
			

	free(var_status);
	free(tab_order_var);
}
