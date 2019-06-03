/*RFL.c
 *Contient des fonction destiné à la résolution des CSP
 *en utilisant l'algorithme du Real Full Look-ahead (RFL)
 */
#include "RFL.h"
#include "AC8.h"


/***
 * Affiche la solution au problème passé en paramètre en utilisant l'algorithme de RFL
 * remarque : utilise les fonctions choix_variables et cherche_domaine_vide du fichier FC.c
 * Paramètre : - probleme correspond au probleme CSP à résoudre
 * 			   - heuristique est l'heuritisque utilisé pour le choix des variables
 * sortie : génère la solution au probleme sur la sortie standart
 */
void RFL (CSP *probleme , int * sol,  HEURISTIQUE heuristique)
{
	printf ("**************RFL**************\n");

	int variable_courante, valeur_courante;
	int affect = 0;			// est égal à 1 si on a réussit à trouver une affectation de la variable courante, 0 sinon
	int *var_status = malloc(sizeof(int) * probleme->max_var);	// var_status[0] = 1 signifie que la 1er variable est déja instancié, 0 sinon)
	int *tab_order_var = malloc(sizeof(int) * probleme->max_var);	// tableau représentant dans l'ordre les variables instanciées (ex : tab_order_var[0] = 5 signifie que la variable 5 est la 1er variable instancié)

	// Initialisation
	for (int i=0; i<probleme->max_var; i++)
		var_status[i] = 0;
	int nb_var_instancie = 0;	// correspond aux nombres de variables instanciées par RFL
    variable_courante = choix_variable(probleme, heuristique, var_status, nb_var_instancie);
    tab_order_var[nb_var_instancie] = variable_courante;

	while (nb_var_instancie < probleme->max_var)
	{
		var_status[variable_courante] = 1;
		affect = 0;

		//on instancie la variable courante
		for (valeur_courante = 0; valeur_courante < probleme->Domain->max_domain && affect == 0; valeur_courante++)
		{
			if (probleme->Domain->domain_matrix[variable_courante][valeur_courante] == 1)
			{				
				if (probleme->Domain->taille_domaine[variable_courante] != 1)
					probleme->Domain->taille_domaine[variable_courante]--;
				
		        for(int d = 0; d < probleme->Domain->max_domain; d++)
		            if(d != valeur_courante && probleme->Domain->domain_matrix[variable_courante][d] == 1)
		                probleme->Domain->domain_matrix[variable_courante][d] = -2;

				// on applique un algorithme de consistance d'arc
				AC8(probleme, var_status);
				
				// si aucun domaine n'est vide on affecte cette valeur à la variable courante, sinon on passe à la prochaine valeur
				if (cherche_domaine_vide(probleme->Domain->taille_domaine, probleme->Domain->max_var, var_status) == 0)
					affect = 1;
				else{
					for(int d = 0; d < probleme->Domain->max_domain; d++)
	            		if(d != valeur_courante && probleme->Domain->domain_matrix[variable_courante][d] == -2)
	                		probleme->Domain->domain_matrix[variable_courante][d] = 1;
				}
			}
		}
		if (affect == 0)
		{
			// si on revient au début et qu'on a pas réussi à affecter la variable alors pas de solution
			if (variable_courante == tab_order_var[0])
            {
                printf("pas de solution\n");
                return;
            }

			// on réinitialise le domaine de la variable courante
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == -2 || probleme->Domain->domain_matrix[variable_courante][j] == -3)
				{
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
					probleme->Domain->taille_domaine[variable_courante]++;
				}

			for (int var_contrainte=0; var_contrainte < probleme->max_var && var_contrainte < variable_courante; var_contrainte++)
		    {
				if (probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte] != NULL)
		        {
					Constraint * current_constraint = probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte];
					for (int i=0; i<current_constraint->max_domain; i++)
						if (probleme->Domain->domain_matrix[var_contrainte][i] == -1)
							probleme->Domain->domain_matrix[var_contrainte][i] == 1;
		        }
		    }

			var_status[variable_courante] = 0;
			// "retour arrière"
			variable_courante = tab_order_var[nb_var_instancie-1];
			nb_var_instancie--;
			
			// on réinitialise le domaine de la variable précédente et on enleve du domaine la précédente valeur inconsistante
			int cpt = 0;
			for (int j=0; j<probleme->Domain->max_domain; j++){
				if (probleme->Domain->domain_matrix[variable_courante][j] == -2)
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
				else if (probleme->Domain->domain_matrix[variable_courante][j] == 1)
					probleme->Domain->domain_matrix[variable_courante][j] = -3;

				if(probleme->Domain->domain_matrix[variable_courante][j] == -3)
					cpt++;
			}
			probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain - cpt;
		}
		else
		{
			// si on a réussit à affecter une valeur à la variable alors on supprime toute les autres valeurs du domaine de la variable
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == 1 && j != valeur_courante-1)
					probleme->Domain->domain_matrix[variable_courante][j] = -2;
			probleme->Domain->taille_domaine[variable_courante] = 1;
			// on choisit la prochaine variable
			variable_courante = choix_variable (probleme, heuristique, var_status, nb_var_instancie);
			nb_var_instancie++;
			tab_order_var[nb_var_instancie] = variable_courante;
		}
	}
	
	// affichage de la solution
	printf("solution : \n");
	for(int i=0; i < probleme->max_var; i++)
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] == 1){
				sol[i] = j;
				printf("x%d = %d \n",i, j);
			}

	// on reinitialise le domaine
	for(int i=0; i < probleme->max_var; i++)
	{
		probleme->Domain->taille_domaine[i] = probleme->Domain->max_domain;
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] != 0)
				probleme->Domain->domain_matrix[i][j] = 1;
	}
}
