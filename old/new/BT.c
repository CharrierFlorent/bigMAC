/*BT.c
 *Contient des fonction destin� � la r�solution des CSP
 *en utilisant l'algorithme du backtrack
 */

#include "BT.h"

/***
 * V�rifie que l'affectation de la variable_courante soit consistante
 * Param�tre : - probleme : le probleme CSP � r�soudre
 * 			   - variable_courante : la variable courante dans l'algorithme de Backtrack
 * 			   - valeur_courante : la valeur courante de la variable courante
 * sortie : retourne 1 si consistant, 0 sinon
 */
int consistant (CSP *probleme, int variable_courante, int valeur_courante)
{
	for (int var_contrainte=0; var_contrainte < probleme->max_var && var_contrainte < variable_courante; var_contrainte++)
    {
		if (probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte] != NULL)
        {
			Constraint * current_constraint = probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte];
			for (int i=0; i<current_constraint->max_domain; i++)
				if (probleme->Domain->domain_matrix[var_contrainte][i] == 1 && current_constraint->relations[valeur_courante][i] == 0)
					return 0;

        }
    }
    return 1;
}

/***
 * Affiche la solution au probl�me pass� en param�tre en utilisant l'algorithme de
 * Backtrack
 * Param�tre : - probleme : le probleme CSP � r�soudre
 * sortie : g�n�re la solution au probleme sur la sortie standart
 */
void BackTrack (CSP *probleme)
{
	printf ("**************BackTrack**************\n");

	int variable_courante, valeur_courante;
	int affect = 0;			//est �gal � 1 si on a r�ussit � trouver une affectation, 0 sinon

	variable_courante=0;
	while (variable_courante < probleme->max_var)
	{
		affect = 0;
		//on instancie une variable
		for (valeur_courante=0; valeur_courante < probleme->Domain->max_domain && affect == 0; valeur_courante++)
		{
			if (probleme->Domain->domain_matrix[variable_courante][valeur_courante] == 1)
				//on cherche une affectation consistante
				if (consistant (probleme, variable_courante, valeur_courante) == 1)
					affect = 1;
		}
		if (affect == 0)
		{
			if (variable_courante == 0)
            {
                printf("pas de solution\n");
                return;
            }
			// si on a pas trouv� d'affectation on revient � la variable pr�c�dente
			variable_courante--;
			// et on r�initialise son domaine
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == -2)
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
				else if (probleme->Domain->domain_matrix[variable_courante][j] == 1)
					probleme->Domain->domain_matrix[variable_courante][j] = -1;
			probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain;

		}
		else
		{
			// si on a r�ussit � affecter une valeur � la variable alors on supprime toute les autres valeurs du domaine de la variable
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == 1 && j != valeur_courante-1)
					probleme->Domain->domain_matrix[variable_courante][j] = -2;
			probleme->Domain->taille_domaine[variable_courante] = 1;
			// et on passe � la variable suivante
			variable_courante++;
		}
	}

	// affichage de la solution
	printf("solution : \n");
	for(int i=0; i < probleme->max_var; i++)
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] == 1)
				printf("x%d = %d \n",i, j);

	// on reinitialise le domaine
	for(int i=0; i < probleme->max_var; i++)
	{
		probleme->Domain->taille_domaine[i] = probleme->Domain->max_domain;
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] != 0)
				probleme->Domain->domain_matrix[i][j] = 1;
	}


}
