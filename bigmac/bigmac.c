#include "bigmac.h"

/***
 * Crée un csp bivalent à partir d'un csp et d'une liste d'affectation
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne un csp binaire bivalent.
 ***/
int noeud_BM = 0;

CSP * create_csp(CSP * csp, int * affectation, int niveau, int size){
    CSP * csp_bivalue = init_empty_csp(niveau+1, csp->Domain->max_domain);
    int val_i, val_j;
    int values_i[2];
    int values_j[2];

    //Taille de tout les domaines à 2
    for(int i = 0; i < niveau+1; i++)
        csp->Domain->taille_domaine[i] = 2;

    //on met à jour nos domaines, on les vide, puis on ajoute les deux valeurs possible pour chaque variable affecté
    for(int i = 0; i < niveau+1; i++)
        for(int j = 0; j < csp->Domain->max_domain; j++){
                csp_bivalue->Domain->domain_matrix[i][j] = 0;
                if(affectation[i] > 0)
                    if(j == (2*(affectation[i]-1)) || j == (2*(affectation[i]-1))+1){
                        csp_bivalue->Domain->domain_matrix[i][j] = 1;
                    }
        }

    //On met à jour les relations, chaque relations est normalement constitué au maximum de 4 valeurs possible (car tous nos domaines sont de taille 2)
    for(int i = 0; i < niveau+1; i++)
        for(int j = 0; j < niveau+1; j++)
            if(affectation[i] >0 && affectation[j] > 0 && i != j){
                if(csp->matrice_contraintes->constraint_matrix[i][j]){ //On ne s'interesse au'aux variables en contraintes dans le csp de base
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = init_constraint(csp->Domain->max_domain);
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->max_domain = csp->Domain->max_domain;

                    for(int k = 0; k < csp->Domain->max_domain;k++)
                        for(int t = 0; t < csp->Domain->max_domain; t++){
                            csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = 0;
                        }

                    /* affectation[i] prend pour valeur un entier entre 1 et taille_domaine/2
                     * cela signifie que 1 correspond aux valeur 0,1; 2 à 2,3; 3 à 4,5 etc...
                     * pour passer de l'un à l'autre, on a donc la formule suivante : 
                     * 2*(affectation[i]-1) donne la 1ere et +1 donne la deuxieme valeur
                     */
                    val_i = 2*(affectation[i]-1);
                    val_j = 2*(affectation[j]-1);

                    values_i[0] = val_i;
                    values_i[1] = (val_i == csp->Domain->max_domain-1) ?  val_i : val_i+1;


                    values_j[0] = val_j;
                    values_j[1] = (val_j == csp->Domain->max_domain-1) ?  val_j : val_j+1;

                    for(int k = 0; k < 2; k++) {
                        for(int t = 0; t < 2; t++){
                            csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->relations[values_i[k]][values_j[t]] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[values_i[k]][values_j[t]];     
                        }
                    }
                }
                else
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = NULL;
            }
            else
                csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = NULL;
    return csp_bivalue;
}

/***
 * Verifie que le csp est SPC
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne 1 si le csp est SPC. 0 sinon
 ***/
int consistent(CSP * csp, int * affectation, int niveau, int size){
	
    CSP * csp_bivalent = create_csp(csp,affectation, niveau, size);
    int vide = 1;

    AC8(csp_bivalent,NULL);
    PC8(csp_bivalent);

    for (int i=0; i < niveau+1; i++){
        vide = 1;
        for (int j=0; j < csp_bivalent->Domain->max_domain; j++)
            if (csp_bivalent->Domain->domain_matrix[i][j] == 1)
                vide = 0;
        if (vide == 1){
            free_csp(csp_bivalent);
            return 0;
        }
    }

    for (int i=0; i < niveau; i++)
        for (int k=0; k < niveau+1; k++){
            if(k == i)
                continue;
            vide = 1;
            if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k] == NULL)
                vide = 0;
            else
            {
                for (int j=0; j < csp_bivalent->Domain->max_domain; j++)
                    for (int l=0; l < csp_bivalent->Domain->max_domain; l++)
                        if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] == 1 && csp_bivalent->Domain->domain_matrix[i][j] && csp_bivalent->Domain->domain_matrix[k][l])
                            vide = 0;
            }
            if (vide == 1){
                free_csp(csp_bivalent);
                return 0;
            }
        }

    free_csp(csp_bivalent);
    return 1;
}

int not_complete(int * array, int size){
    for(int i =0; i < size; i++)
        if(array[i] == 0)
            return 1;
    return 0;
}

/***
 * Affecte à la prochaine variables une paire de valeurs
 * paramètre   - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur courante dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne 1 si l'affectation est possible. 0 sinon (domaine exploré entierement)
 ***/
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

/***
 * Résoud le csp bivalent final
 * paramètre : - csp : un csp
 ***/
void solve_csp(CSP * csp){
    CSP * csp2 = create_csp_by_copy(csp);
    int * inst = calloc(csp->max_var,sizeof(int));
    //Forward_Checking(csp,inst, MIN_DOMAINE);
	int * var = calloc(csp->max_var,sizeof(int));

    if(FC(csp2, inst, var, 0)){
        printf("solution : \n");
        for(int i=0; i < csp->max_var; i++)
            printf("x%d = %d \n",i, inst[i]);


        if(verify(csp2,inst))
            printf("BM : correct!\n");
        else
            printf("BM : Incorrect!\n");
    }
    free_csp(csp2);
    free(inst);
    free(var);
}

/***
 * Execute l'algorithme du bigMAC sur un csp donné
 * paramètre : - csp : un csp
 ***/
void bigmac(CSP *csp){
    printf ("\n*****************************BIGMAC***************************\n");
    CSP * bigmac_csp;
    int niveau = 0;
    int succes_affectation, succes_consistence = 0;
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
        //printf("niveau %d \n", niveau);
        succes_affectation = affecter(affectation, niveau, taille_domaine);
        if(!succes_affectation){
            affectation[niveau] = 0;
            niveau--;
            if(niveau < 0){
                free(affectation);
                printf("pas de solutions\n");
                return;
            }
        }
        else
        {
            noeud_BM++;
            succes_consistence = consistent(csp, affectation, niveau, taille_domaine);
            if(succes_consistence){
                niveau++;
                
            }
            else{
                continue;
            }
        }
    }

    bigmac_csp = create_csp(csp, affectation, niveau-1, taille_domaine);
    solve_csp(bigmac_csp);


    free_csp(bigmac_csp);
    free(affectation);
}


