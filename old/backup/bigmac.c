#include "bigmac.h"

CSP * create_csp(CSP * csp, int * affectation, int niveau, int size){
    CSP * csp_bivalue = init_empty_csp(niveau+1, csp->Domain->max_domain);

    for(int i = 0; i < niveau+1; i++)
        csp->Domain->taille_domaine[i] = 2;

    for(int i = 0; i < niveau+1; i++)
        for(int j = 0; j < csp->Domain->max_domain; j++){
                csp_bivalue->Domain->domain_matrix[i][j] = 0;
                if(affectation[i] > 0)
                    if(j == (2*(affectation[i]-1)) || j == (2*(affectation[i]-1))+1){
                        csp_bivalue->Domain->domain_matrix[i][j] = 1;
                    }
        }

	for(int i = 0; i < niveau+1; i++)
        for(int j = 0; j < niveau+1; j++)
            if(affectation[i] >0 && affectation[j] > 0){
                if(csp->matrice_contraintes->constraint_matrix[i][j]){
                    //printf ("contrainte entre %d et %d\n", i, j);
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = init_constraint(csp->Domain->max_domain);
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->max_domain = csp->Domain->max_domain;

                    for(int k = 0; k < csp->Domain->max_domain;k++)
                        for(int t = 0; t < csp->Domain->max_domain; t++){
                            //printf ("k = %d, t = %d\n",k,t);
                            csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = 0;
                            //printf ("if = %d\n", ( (k == (2*(affectation[niveau]-1))) || (k == (2*(affectation[niveau]-1)+1))) && (t == (2*(affectation[niveau]-1)) || (t == (2*(affectation[niveau]-1)+1))));
                            if(( (k == (2*(affectation[i]-1))) || (k == (2*(affectation[j]-1)+1))) && (t == (2*(affectation[i]-1)) || (t == (2*(affectation[j]-1)+1)))){
                                csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[k][t];
                            }
                        }
                }
                else
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = NULL;
            }
            else
                csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = NULL;
    //printf("niveau %d\n",niveau);
    //print_matrix(csp_bivalue->Domain->domain_matrix, niveau+1, csp->Domain->max_domain);
    return csp_bivalue;
}


/*marche pas vraiment encore, à la création du csp il faut bien choisir les valeurs, à remodifier, pour l'instant à tester que sur csp
  au domaine de taille pair pour eviter les soucis*/
int consistent(CSP * csp, int * affectation, int niveau, int size){
    //printf("creatipon?\n");
    CSP * csp_bivalue = create_csp(csp,affectation, niveau, size);
    //printf ("gello...\n");
    int vide = 1;
    //print_relation(csp_bivalue);
    PC8(csp_bivalue);
    AC8(csp_bivalue,NULL);
    //print_relation(csp_bivalue);
    //printf("\nend\n\n");
    //printf("fin AC8");
    /*for(int i=0; i < csp_bivalue->max_var; i++)
		for (int j=0; j<csp_bivalue->Domain->max_domain; j++)
				printf("domain[%d][%d] = %d \n",i, j, csp_bivalue->Domain->domain_matrix[i][j]);*/
    //print_relation(csp_bivalue);
    /* soit modifier AC8/PC8, soit tester ici les domaines/relations ,si l'un est vide, alors c'est plus consistent et on retourne 0 */

    for (int i=0; i < niveau+1; i++)
    {
		//printf ("i = %d\n",i);
		vide = 1;
		for (int j=0; j < csp_bivalue->Domain->max_domain; j++)
			if (csp_bivalue->Domain->domain_matrix[i][j] == 1)
				vide = 0;
		if (vide == 1)
			return 0;
	}

	for (int i=0; i < niveau+1; i++)
		for (int k=0; k < niveau+1; k++)
		{
			//printf ("k = %d\n",k);
			vide = 1;
			if (csp_bivalue->matrice_contraintes->constraint_matrix[i][k] == NULL)
				vide = 0;
			else
			{
				for (int j=0; j < csp_bivalue->Domain->max_domain; j++)
					for (int l=0; l < csp_bivalue->Domain->max_domain; l++)
						if (csp_bivalue->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] == 1)
							vide = 0;
			}
			if (vide == 1)
				return 0;
		}

    free_csp(csp_bivalue);
    return 1;
}

int not_complete(int * array, int size){
    for(int i =0; i < size; i++)
        if(array[i] == 0)
            return 1;
    return 0;
}

int affecter(int * affectation, int niveau, int size){
    if(size%2 == 0){
        if(affectation[niveau]+1 > (int)size/2)
            return 0;
    }else{
        if(affectation[niveau]+1 > (int)(size + 2 - 1)/2) //arrondi vers le haut
            return 0;
    }
    affectation[niveau]++;
    return 1;
}

void solve_csp(CSP * csp){
    BackTrack(csp);
}

int bigmac(CSP *csp){
	//printf ("\n*****************************BIGMAC***************************\n");
    CSP * bigmac_csp;
    int niveau = 0;
    int noeud = 0;
    int succes_affectation, succes_consistence;
    int max_var = csp->max_var;
    int taille_domaine = csp->Domain->max_domain;
    int * affectation; /* Un tableau d'affectation, pour l'instant on ne va pas implementer d'heuristique de choix de pair de variables
                        * On a donc que chaque variables a pour domaine taille_domaine/2 valeur possible.
                        * ainsi si elle vaut 1, alors on considerera le couple des 2 premiere valeurs, si elle vaut 3 le 3eme couple
                        * de valeur etc... 0 signifie non affecté.
                        */
    affectation = malloc(csp->max_var*sizeof(int));
    for(int i = 0; i < max_var; i++){affectation[i] = 0;}

    /* Algo : On affecte une valeur. Si ce n'est pas possible (domaine etudié en entier
     * on arrete, pas de solutions.
     * On teste la double consistence sur le sous csp ensuite. Si ce n'est pas consistent
     * on test la valeur suivante, s'il n'y a plus de valeur suivante possible, on remonte d'un cran
     * On s'arrete quand on a explorer tout l'arbre
     */
    while(not_complete(affectation, max_var) || !succes_consistence){
		//printf ("niveau = %d\n", niveau);
		succes_affectation = affecter(affectation, niveau, taille_domaine);
        //printf ("succes_affectation = %d\n", succes_affectation);
        if(!succes_affectation){
            affectation[niveau] = 0;
            niveau--;
            if(niveau < 0){
                //printf("pas de solutions\n");
                return noeud;
            }
        }
        else
        {
			//printf ("blouf!!!!!!!!!!!!\n");

			succes_consistence = consistent(csp, affectation, niveau, taille_domaine);
			//printf ("succes consistence = %d\n", succes_consistence);
			if(succes_consistence){
				niveau++;
                noeud++;
			}
			else{
				continue;
			}
		}
    }
    //printf ("I'm out ! \n");
    //printf ("niveau = %d\n", niveau);
    //for(int i = 0; i < csp->max_var;i++)
        //printf("%d ",affectation[i]);
    //printf("\n");
    bigmac_csp = create_csp(csp, affectation, niveau-1, taille_domaine);
    //printf ("creer\n");
	//print_matrix(bigmac_csp->Domain->domain_matrix, bigmac_csp->max_var, bigmac_csp->Domain->max_domain);
	//print_relation(bigmac_csp);
    solve_csp(bigmac_csp);
    free(bigmac_csp);

    return noeud;
}


