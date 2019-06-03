#include "bigmac.h"


/***
 * Crée un csp bivalent à partir d'un csp et d'une liste d'affectation
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne un csp binaire bivalent.
 ***/
CSP * create_csp(CSP * csp, int * affectation, int niveau, int size){
    CSP * csp_bivalent = init_empty_csp(csp->max_var, csp->Domain->max_domain);
    for(int i = 0; i < csp->max_var; i++)
        for(int j = 0; j < csp->Domain->max_domain; j++){
                csp_bivalent->Domain->domain_matrix[i][j] = 0;
                if(affectation[i] > 0)
                    if(j == (2*(affectation[i]-1)) || j == (2*(affectation[i]-1))+1)
                        csp_bivalent->Domain->domain_matrix[i][j] = 1;
        }

    for(int i = 0; i < csp->max_var; i++)
        for(int j = 0; j < csp->max_var; j++)
            if(affectation[i] >0 && affectation[j] > 0){
                if(csp->matrice_contraintes->constraint_matrix[i][j]){
                    csp_bivalent->matrice_contraintes->constraint_matrix[i][j] = init_constraint(csp->Domain->max_domain);
                    csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->max_domain = csp->Domain->max_domain;
                    for(int k = 0; k < csp->Domain->max_domain;k++)
                        for(int t = 0; t < csp->Domain->max_domain; t++){
                            csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = 0;
                            if(( (k == (2*(affectation[niveau]-1))) || (k == (2*(affectation[niveau]-1)+1))) && (t == (2*(affectation[niveau]-1)) || (t == (2*(affectation[niveau]-1)+1))))
                                csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[k][t];
                        }
                }
                else
                    csp_bivalent->matrice_contraintes->constraint_matrix[i][j] = NULL;
            }
            else
                csp_bivalent->matrice_contraintes->constraint_matrix[i][j] = NULL;
    return csp_bivalent;
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


    for (int i=0; i < niveau+1; i++)
    {
        //printf ("i = %d\n",i);
        vide = 1;
        for (int j=0; j < csp_bivalent->Domain->max_domain; j++)
            if (csp_bivalent->Domain->domain_matrix[i][j] == 1)
                vide = 0;
        if (vide == 1)
            return 0;
    }

    for (int i=0; i < niveau; i++)
        for (int k=0; k < niveau+1; k++)
        {
            //printf ("k = %d\n",k);
            vide = 1;
            if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k] == NULL)
                vide = 0;
            else
            {
                for (int j=0; j < csp_bivalent->Domain->max_domain; j++)
                    for (int l=0; l < csp_bivalent->Domain->max_domain; l++)
                        if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] == 1)
                            vide = 0;
            }
            if (vide == 1)
                return 0;
        }

    //free_csp(csp_bivalent);
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
 * Permet de tester si une solution est valide.
 * paramètre : - csp : un csp
 *             - solution : un tableau contenant une solutions au csp que l'on veut tester
 ***/
int verify2(CSP * csp, int * solution){
    for(int i = 0; i < csp->max_var; i++)
        for(int j = 0; j < csp->max_var; j++){
                if(csp->matrice_contraintes->constraint_matrix[i][j]){
                    if(!csp->matrice_contraintes->constraint_matrix[i][j]->relations[solution[i]][solution[j]])
                        return 0;
                }
        }
    return 1;
}

/***
 * Résoud le csp bivalent final
 * paramètre : - csp : un csp
 ***/
void solve_csp(CSP * csp){
    CSP * csp2 = create_csp_by_copy(csp);
    int * inst = calloc(csp->max_var,sizeof(int));
    Forward_Checking(csp,inst, MIN_DOMAINE);


        if(verify2(csp2,inst))
            printf("BM : correct!\n");
        else
            printf("BM : Incorrect!\n");
    

}

/***
 * Execute l'algorithme du bigMAC sur un csp donné
 * paramètre : - csp : un csp
 ***/
void bigmac(CSP *csp){
    printf ("\n*****************************BIGMAC***************************\n");
    CSP * bigmac_csp;
    int niveau = 0;
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
        succes_affectation = affecter(affectation, niveau, taille_domaine);
        if(!succes_affectation){
            affectation[niveau] = 0;
            niveau--;
            if(niveau < 0){
                printf("pas de solutions\n");
                return;
            }
        }
        else
        {
            succes_consistence = consistent(csp, affectation, niveau, taille_domaine);
            if(succes_consistence)
                niveau++;
            else{
                continue;
            }
        }
    }

    bigmac_csp = create_csp(csp, affectation, niveau-1, taille_domaine);
    solve_csp(bigmac_csp);
}


