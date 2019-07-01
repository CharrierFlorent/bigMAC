/*FC.c
 *Contient des fonction destiné à la résolution des CSP
 *en utilisant l'algorithme du Forward Checking
 */
#include "FC.h"
 extern FILE * glb_output_file;

int noeud_FC = 0;
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
		//if (status[i] == 0)
		//{
			if (taille_domaine[i] == 0)
				return 1;
		//}
	}
	return 0;
}

/***
 * Détermine la prochaine variable courante par rapport à l'heuristique choisie
 * paramètres : - probleme : le probleme CSP à résoudre
 * 				- heuristique : l'heuristique choisie pour le choix de la variable
 * 				- status : un tableau de booleen qui permet de savoir si une variable est déja instancié
 * 				- nb_var_instancie : le nombre de variable déjà instancié
 * sortie : retourne la prochaine variable a instancié ou -1 si rien n'a pu etre affecté
 */
int choix_variable (CSP *probleme, HEURISTIQUE heuristique, int *status, int nb_var_instancie)
{
	int var = -1, min, nb_contraintes;

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
					if(nb_contraintes == 0)
						nb_contraintes = probleme->max_var;
					if (min > probleme->Domain->taille_domaine[i] / nb_contraintes)
					{
						var = i;
						min = probleme->Domain->taille_domaine[i] / nb_contraintes;
					}
				}
			}
			break;
		case PROFONDEUR:
			var = nb_var_instancie;
			break;
		default:
			var = nb_var_instancie + 1;
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
				for (int index=0; index < probleme->Domain->max_domain; index++)
					if ( probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte]->relations[valeur_courante][index] == 0 && probleme->Domain->domain_matrix[var_contrainte][index] == -1)
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
			if (probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte])
			{
				// pour toutes les valeurs du domaine des autres variables on enlève les valeurs devenues inconsitantes
				for (int index=0; index < probleme->Domain->max_domain; index++)
					if (probleme->matrice_contraintes->constraint_matrix[variable_courante][var_contrainte]->relations[valeur_courante][index] == 0 && probleme->Domain->domain_matrix[var_contrainte][index] == 1)
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
void Forward_Checking (CSP *probleme , int * solution, HEURISTIQUE heuristique)
{
	fprintf(glb_output_file, "**************Forward Checking**************\n");

	int variable_courante, valeur_courante;
	int affect = 0;			// est égal à 1 si on a réussit à trouver une affectation de la variable courante, 0 sinon
	int *var_status = malloc(sizeof(int) * probleme->max_var);	// var_status[0] = 1 signifie que la 1er variable est déja instancié, 0 sinon)
	int *tab_order_var = malloc(sizeof(int) * probleme->max_var);	// tableau représentant dans l'ordre les variables instanciées (ex : tab_order_var[0] = 5 signifie que la variable 5 est la 1er variable instancié)

	// Initialisation
	for (int i=0; i<probleme->max_var; i++){
		var_status[i] = 0;
        tab_order_var[i] = -1;
	}
	int nb_var_instancie = 0; 	// correspond aux nombres de variables instanciées par Forward Checking
    variable_courante = 0;//choix_variable(probleme, heuristique, var_status, nb_var_instancie);
    tab_order_var[nb_var_instancie] = variable_courante;
	while (nb_var_instancie < probleme->max_var)
	{
		//fprintf(glb_output_file,"variable_courante %d\n", variable_courante);
		var_status[variable_courante] = 1;
		affect = 0;
		//on instancie la variable courante
		for (valeur_courante = 0; valeur_courante < probleme->Domain->max_domain; valeur_courante++)
		{
		    //fprintf(glb_output_file,"variable %d\n", variable_courante);
			if (probleme->Domain->domain_matrix[variable_courante][valeur_courante] == 1)
			{
				probleme->Domain->taille_domaine[variable_courante]--;

				// on filtre les domaines
				filtre_domaine (probleme, variable_courante, valeur_courante, var_status);

                // si aucun domaine n'est vide on affecte cette valeur à la variable courante, sinon on réinitialise les domaines et on passe à la prochaine valeur
				if (cherche_domaine_vide(probleme->Domain->taille_domaine, probleme->Domain->max_var, var_status) == 0){
					affect = 1;
					break;
				}
				else
                    reinitialise_domaine (probleme, variable_courante, valeur_courante, var_status);
			}
		}
		if (affect == 0)
		{
			//fprintf(glb_output_file,"affectation ratée %d nb_var_instancie %d\n", variable_courante, nb_var_instancie);
			// si on revient au début et qu'on a pas réussi à affecter la variable alors pas de solution
			if (variable_courante == tab_order_var[0])
            {
                fprintf(glb_output_file,"pas de solution\n");
                return;
            }
			var_status[variable_courante] = 0;
			// "retour arrière"

            for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == -3)
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
			probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain;
            //tab_order_var[nb_var_instancie] = 0;
			variable_courante = tab_order_var[nb_var_instancie-1];
			tab_order_var[nb_var_instancie] = -1;
			//fprintf(glb_output_file,"variable %d\n", variable_courante);
			//fprintf(glb_output_file,"affecte tableau %d\n", variable_courante);
			if(var_status[variable_courante]){
                var_status[variable_courante] = 0;
                nb_var_instancie--;
			}
            /*for (int j=0; j<probleme->Domain->max_domain; j++)
                if (probleme->Domain->domain_matrix[variable_courante][j] == 1){
					probleme->Domain->domain_matrix[variable_courante][j] = -2;
                }
			probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain;*/
			// on réinitialise le domaine de la variable précédente et on enleve du domaine la précédente valeur inconsistante
			int taille = 0;
            for (int j=0; j<probleme->Domain->max_domain; j++){
				if (probleme->Domain->domain_matrix[variable_courante][j] == -2){
					probleme->Domain->domain_matrix[variable_courante][j] = 1;
				}
            }
			for (int j=0; j<probleme->Domain->max_domain; j++){
				if (probleme->Domain->domain_matrix[variable_courante][j] == 1){
					//reinitialise_domaine (probleme, variable_courante, j, var_status);
					probleme->Domain->domain_matrix[variable_courante][j] = -3;
				}

                if(probleme->Domain->domain_matrix[variable_courante][j] == -3){
                    taille++;
                    //fprintf(glb_output_file,"aucun ???%d\n " ,taille);
                }
            }

			probleme->Domain->taille_domaine[variable_courante] = probleme->Domain->max_domain-taille;
			//fprintf(glb_output_file,"taille %d\n", probleme->Domain->taille_domaine[variable_courante]);
			if(probleme->Domain->taille_domaine[variable_courante] == 0){
				fprintf(glb_output_file,"pas de solution\n");
				return;
			}

		}
		else
		{
			//fprintf(glb_output_file,"affectation reussis %d nb_var_instancie %d\n", variable_courante, nb_var_instancie);
			// si on a réussit à affecter une valeur à la variable alors on supprime toute les autres valeurs du domaine de la variable
			for (int j=0; j<probleme->Domain->max_domain; j++)
				if (probleme->Domain->domain_matrix[variable_courante][j] == 1 && j != valeur_courante){ // attention ici pas sur
					probleme->Domain->domain_matrix[variable_courante][j] = -2;
				}
			solution[variable_courante] = valeur_courante;
			probleme->Domain->taille_domaine[variable_courante] = 1;
			// on choisit la prochaine variable
			variable_courante = choix_variable (probleme, heuristique, var_status, nb_var_instancie);
			if(variable_courante == -1){
                if(nb_var_instancie == probleme->max_var-1)
                    break;
                fprintf(glb_output_file,"pas de sol\n");
                return;
			}
			
			tab_order_var[nb_var_instancie] = variable_courante;
			nb_var_instancie++;
			//fprintf(glb_output_file,"affecté fonction %d\n", variable_courante);
		}
	}

	// affichage de la solution
	fprintf(glb_output_file,"solution : \n");
	for(int i=0; i < probleme->max_var; i++)
		fprintf(glb_output_file,"x%d = %d \n",i, solution[i]);
			

	// on reinitialise le domaine
	for(int i=0; i < probleme->max_var; i++)
	{
		probleme->Domain->taille_domaine[i] = probleme->Domain->max_domain;
		for (int j=0; j<probleme->Domain->max_domain; j++)
			if (probleme->Domain->domain_matrix[i][j] != 0)
				probleme->Domain->domain_matrix[i][j] = 1;
	}
}

int check_forward(CSP * csp, int * var_status, int variable_courante, int valeur_courante, int profondeur){
	int consistant_futur = 1;
	for(int i = 0; i < csp->max_var;i++){
		if(var_status[i] == 0 && i != variable_courante && csp->matrice_contraintes->constraint_matrix[variable_courante][i]){
			while(consistant_futur){
				for(int v = 0; v < csp->Domain->max_domain; v++){
					if(csp->Domain->domain_matrix[i][v] == 1){
						if(csp->matrice_contraintes->constraint_matrix[variable_courante][i]->relations[valeur_courante][v] == 0){
							csp->Domain->domain_matrix[i][v] = -(profondeur+2);
							csp->Domain->taille_domaine[i]--;
						}
					}
				}
				if(csp->Domain->taille_domaine[i] == 0){
					consistant_futur = 0;
					return consistant_futur;
				}
				break;
			}
		}
	}
	return consistant_futur;
}

void restore_domain(CSP * csp, int * var_status, int variable_courante, int profondeur){
	for(int i = 0; i < csp->max_var;i++){
		if(var_status[i] == 0 && i != variable_courante && csp->matrice_contraintes->constraint_matrix[variable_courante][i]){
				for(int v = 0; v < csp->Domain->max_domain; v++){
					if(csp->Domain->domain_matrix[i][v] == -(profondeur+2)){
							csp->Domain->domain_matrix[i][v] = 1;
							csp->Domain->taille_domaine[i]++;
					}
				}
		}
	}	
}

int FC(CSP * csp, int * solution, int * var_status, int profondeur){

	if(profondeur == csp->max_var){
		return 1;
	}
	else{
		var_status[profondeur] = 1;
		int variable_courante = profondeur;
		for(int valeur_courante = 0; valeur_courante < csp->Domain->max_domain; valeur_courante++){
			if (csp->Domain->domain_matrix[variable_courante][valeur_courante] == 1){
				noeud_FC++;
             	if (check_forward(csp, var_status, variable_courante, valeur_courante, profondeur)){
					solution[profondeur] = valeur_courante;
					if(FC(csp, solution, var_status, profondeur+1))
						return 1;	
				}
                restore_domain(csp, var_status, variable_courante, profondeur);
			}
		}
		var_status[profondeur] = 0;
	}
	return 0;
}