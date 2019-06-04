/*FC.c
 *Contient des fonction destiné à la résolution des CSP
 *en utilisant l'algorithme du Forward Checking
 */
#include "FC.h"



/***
 * Parcourt chaque variable et cherche si une variable a un domaine vide
 * paramètres : - taille_domaine : un tableau qui contient la taille du domaine de chaque variable
 * 				- max_var : le nombre de variable maximale
 * 				- status : un tableau de booleen qui permet de savoir si une variable est déja instancié
 * sortie : retourne 1 si un domaine est vide, 0 sinon
 */
int cherche_domaine_vide (int *taille_domaine, int max_var, int *status)
{
	for (int i=0; i<max_var; i++)
	{
		// si la variable n'est pas déja instanciée alors on regarde si son domaine est vide
		if (status[i] == 0)
		{
			if (taille_domaine[i] == 0)
				return 1;
		}
	}
	return 0;
}

/***
 * Détermine la prochaine variable courante par rapport à l'heuristique choisie
 * paramètres : - probleme : le probleme CSP à résoudre
 * 				- heuristique : l'heuristique choisie pour le choix de la variable
 * 				- status : un tableau de booleen qui permet de savoir si une variable est déja instancié
 * 				- nb_var_instancie : le nombre de variable déjà instancié
 * sortie : retourne la prochaine variable a instancié
 */
int choix_variable (CSP *probleme, HEURISTIQUE heuristique, int *status, int nb_var_instancie)
{
	int var, min, nb_contraintes;
	switch (heuristique)
	{
		case MIN_DOMAINE:
			min = probleme->Domain->max_domain+1;
			for (int i=0; i<probleme->max_var; i++)
			{
				// si la variable n'est pas déja instanciée alors on regarde la taille de son domaine courant
				if (status[i] == 0)
				{
					if (min > probleme->Domain->taille_domaine[i])
					{
						var = i;
						min = probleme->Domain->taille_domaine[i];
					}
				}
			}
			break;
		case DOMAINE_DEGRE:
			min = probleme->Domain->max_domain+1;
			for (int i=0; i<probleme->max_var; i++)
			{
				nb_contraintes = 0;
				// si la variable n'est pas déja instanciée alors on regarde son ratio domaine/degré
				if (status[i] == 0)
				{
					for (int j=0; j<probleme->max_var; j++)
						if (probleme->matrice_contraintes->constraint_matrix[i][j] != NULL)
							nb_contraintes++;
					if (min > probleme->Domain->taille_domaine[i] / nb_contraintes)
					{
						var = i;
						min = probleme->Domain->taille_domaine[i] / nb_contraintes;
					}
				}
			}
			break;
		default:
			if (nb_var_instancie == 0)
				var = 0;
			else var = nb_var_instancie + 1;
			break;

	}
	return var;
}

/***
 * Réinitialise les valeurs du domaine des variables qui ont été filtrés par l'affectation
 * de la variable courante mais qui sont redevenus consistantes
 * paramètres : - probleme correspond au probleme CSP à résoudre
 * 				- variable_courante est la variable courante dans l'algorithme de Forward_Checking
 * 				- valeur_courante est la valeur courante de la variable courante
 * 				- status : un tableau de booleen qui permet de savoir si une variable est déja instancié
 */
void reinitialise_domaine (CSP *probleme, int variable_courante, int valeur_courante, int *status)
{
	for (int var_contrainte = 0; var_contrainte < probleme->max_var; var_contrainte++)
	{
		if (status[var_contrainte] == 0)
			if (probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte] != NULL)
			{
				Constraint * current_constraint = probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte];
				for (int index=0; index < current_constraint->max_domain; index++)
					if (current_constraint->relations[valeur_courante][index] == 0 && probleme->Domain->domain_matrix[var_contrainte][index] == -1)
					{
						probleme->Domain->domain_matrix[var_contrainte][index] = 1;
						probleme->Domain->taille_domaine[var_contrainte]++;
					}
			}
	}
}

/***
 * Filtre les domaines des variables aprés l'affectation de la variable courante
 * paramètres : - probleme correspond au probleme CSP à résoudre
 * 				- variable_courante est la variable courante dans l'algorithme de Forward_Checking
 * 				- valeur_courante est la valeur courante de la variable courante
 * 				- status : un tableau de booleen qui permet de savoir si une variable est déja instancié
 */
void filtre_domaine (CSP *probleme, int variable_courante, int valeur_courante, int *status)
{
	// on filtre les domaines
	for (int var_contrainte = 0; var_contrainte < probleme->max_var; var_contrainte++)
    {
		if (status[var_contrainte] == 0)
		// s'il existe une contrainte entre la variable courante et les autres variables
			if (probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte] != NULL)
			{
				Constraint * current_constraint = probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte];
				// pour toutes les valeurs du domaine des autres variables on enlève les valeurs devenues inconsitantes
				for (int index=0; index < current_constraint->max_domain; index++)
					if (current_constraint->relations[valeur_courante][index] == 0 && probleme->Domain->domain_matrix[var_contrainte][index] == 1)
					{
						probleme->Domain->domain_matrix[var_contrainte][index] = -1;
						probleme->Domain->taille_domaine[var_contrainte]--;
					}
			}
    }
}

/***
 * Affiche la solution au problème passé en paramètre en utilisant l'algorithme de
 * Forward Checking
 * Paramètre : - probleme correspond au probleme CSP à résoudre
 * 			   - heuristique est l'heuritisque utilisé pour le choix des variables
 * sortie : génère la solution au probleme sur la sortie standart
 */
void Forward_Checking (CSP *probleme , HEURISTIQUE heuristique)
{
	//printf ("**************Forward Checking**************\n");

	int variable_courante, valeur_courante;
	int affect = 0;			// est égal à 1 si on a réussit à trouver une affectation de la variable courante, 0 sinon
	int *var_status = malloc(sizeof(int) * probleme->max_var);	// var_status[0] = 1 signifie que la 1er variable est déja instancié, 0 sinon)
	int *tab_order_var = malloc(sizeof(int) * probleme->max_var);	// tableau représentant dans l'ordre les variables instanciées (ex : tab_order_var[0] = 5 signifie que la variable 5 est la 1er variable instancié)

	// Initialisation
	for (int i=0; i<probleme->max_var; i++)
		var_status[i] = 0;
	int nb_var_instancie = 0; 	// correspond aux nombres de variables instanciées par Forward Checking
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
				probleme->Domain->taille_domaine[variable_courante]--;

				// on filtre les domaines
				filtre_domaine (probleme, variable_courante, valeur_courante, var_status);

                // si aucun domaine n'est vide on affecte cette valeur à la variable courante, sinon on réinitialise les domaines et on passe à la prochaine valeur
				if (cherche_domaine_vide(probleme->Domain->taille_domaine, probleme->Domain->max_var, var_status) == 0)
					affect = 1;
				else reinitialise_domaine (probleme, variable_courante, valeur_courante, var_status);
			}
		}
		if (affect == 0)
		{
			// si on revient au début et qu'on a pas réussi à affecter la variable alors pas de solution
			if (variable_courante == tab_order_var[0])
            {
                //printf("pas de solution\n");
                return;
            }
			var_status[variable_courante] = 0;
			// "retour arrière"
			variable_courante = tab_order_var[nb_var_instancie-1];
			var_status[variable_courante] = 0;
			nb_var_instancie--;
			// on réinitialise le domaine de la variable précédente et on enleve du domaine la précédente valeur inconsistante
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == -2)
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
				else if (probleme->Domain->domain_matrix[variable_courante][j] == 1)
					probleme->Domain->domain_matrix[variable_courante][j] = -2;
			probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain;
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
	//printf("solution : \n");
	for(int i=0; i < probleme->max_var; i++)
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] == 1)
				//printf("x%d = %d \n",i, j);

	// on reinitialise le domaine
	for(int i=0; i < probleme->max_var; i++)
	{
		probleme->Domain->taille_domaine[i] = probleme->Domain->max_domain;
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] != 0)
				probleme->Domain->domain_matrix[i][j] = 1;
	}
}
