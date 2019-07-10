#include "bigmac.h"

 extern FILE * glb_output_file;
int noeud_BM = 0;

void reload_relation(CSP * csp_bivalent, int variable_courante, int profondeur){
    for(int i = 0; i < csp_bivalent->max_var; i++)
        for(int k = 0; k < csp_bivalent->max_var; k++){
            if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k])
                for (int j=0; j < csp_bivalent->Domain->max_domain; j++)
                    for (int l=0; l < csp_bivalent->Domain->max_domain; l++)
                        if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] == -(OFFSET+profondeur)){
                            csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] = 1;
                        }
        }
}

/***
 * Crée un csp bivalent à partir d'un csp et d'une liste d'affectation
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne un csp binaire bivalent.
 ***/
CSP * create_csp(CSP * csp, int * affectation, int niveau, int size){
    CSP * csp_bivalue = init_empty_csp(niveau+1, csp->Domain->max_domain);
    int val_i, val_j;
    int values_i[2];
    int values_j[2];

    //Taille de tout les domaines à 2
    for(int i = 0; i < niveau+1; i++)
        csp->Domain->taille_domaine[i] = csp->Domain->max_domain;

    //on met à jour nos domaines, on les vide, puis on ajoute les deux valeurs possible pour chaque variable affecté
    for(int i = 0; i < niveau+1; i++)
        for(int j = 0; j < csp->Domain->max_domain; j++){
                csp_bivalue->Domain->domain_matrix[i][j] = 0;
                csp->Domain->taille_domaine[i]--;
                if(affectation[i] > 0)
                    if(j == (2*(affectation[i]-1)) || j == (2*(affectation[i]-1))+1){
                        csp_bivalue->Domain->domain_matrix[i][j] = 1;
                        csp->Domain->taille_domaine[i]++;
                    }
        }
       int non_vide;
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
                else{
                    //csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = NULL;
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = init_constraint(csp->Domain->max_domain);
                    csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->max_domain = csp->Domain->max_domain;
                    for(int k = 0; k < csp->Domain->max_domain; k++) {
                        for(int t = 0; t < csp->Domain->max_domain; t++){
                            csp_bivalue->matrice_contraintes->constraint_matrix[i][j]->relations[k][t] = 1;
                        }
                    }
                }
            }
            else{
                csp_bivalue->matrice_contraintes->constraint_matrix[i][j] = NULL;
            }
    return csp_bivalue;
}

/***
 * Verifie la consistence d'arc du csp
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne 1 si le csp est SPC. 0 sinon
 ***/
int AC(CSP * csp, int * affectation, int current_var, int niveau){

    int element_1 = 2*(affectation[current_var]-1);
    int element_2 = (element_1 == csp->Domain->max_domain-1) ? element_1 : element_1+1;
    
    reduce_domain(csp, current_var, element_1);
    csp->Domain->domain_matrix[current_var][element_2] = 1;
    csp->Domain->taille_domaine[current_var]++;
    // on applique un algorithme de consistance d'arc
    AC8(csp, OFFSET+niveau, NULL); 
    if (cherche_domaine_vide(csp->Domain->taille_domaine, csp->Domain->max_var, NULL)){
        return 0;
    }   
    return 1;

}

/***
 * Test de SPC. Crée un sous csp binaire, puis execute AC8 et PC8 dessus
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne 1 si le csp est SPC. 0 sinon
 ***/
int SPC(CSP * csp, int * affectation, int current_var, int niveau, int size){
    CSP * csp_bivalent = create_csp(csp,affectation, niveau, size);
    int vide = 1;
    print_matrix(csp_bivalent->Domain->domain_matrix,csp_bivalent->Domain->max_var,csp_bivalent->Domain->max_domain);
    PC8(csp_bivalent,OFFSET+niveau);
    AC8(csp_bivalent,OFFSET+niveau,NULL);
    //print_relation(csp_bivalent);
    //verification domaine et relation vide
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
                        if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] == 1 && csp_bivalent->Domain->domain_matrix[i][j] == 1 && csp_bivalent->Domain->domain_matrix[k][l] == 1)
                            vide = 0;
            }
            if (vide == 1){
                free_csp(csp_bivalent);
                printf("vide \n");
                return 0;
            }
        }

    //modifier les valeurs du vrai csp
    for (int i=0; i < niveau+1; i++){
        for (int j=0; j < csp_bivalent->Domain->max_domain; j++){
            csp->Domain->domain_matrix[i][j] = csp_bivalent->Domain->domain_matrix[i][j];
            //printf("AC8 filtering : %d\n",csp_bivalent->Domain->domain_matrix[i][j]);
        }
    }

    for (int i=0; i < niveau; i++)
        for (int k=0; k < niveau+1; k++){
            if(k == i)
                continue;
            if (csp_bivalent->matrice_contraintes->constraint_matrix[i][k] && csp->matrice_contraintes->constraint_matrix[i][k]){
                for (int j=0; j < csp_bivalent->Domain->max_domain; j++)
                    for (int l=0; l < csp_bivalent->Domain->max_domain; l++){
                    	if(csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] == -(niveau+OFFSET))
                        	csp->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] = csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l];
                    }
            }
            else if(csp_bivalent->matrice_contraintes->constraint_matrix[i][k]){
            	    csp->matrice_contraintes->constraint_matrix[i][k] = init_constraint(csp->Domain->max_domain);
                    csp->matrice_contraintes->constraint_matrix[i][k]->max_domain = csp->Domain->max_domain;
                    for(int j = 0; j < csp->Domain->max_domain; j++) {
                        for(int l = 0; l < csp->Domain->max_domain; l++){
                            csp->matrice_contraintes->constraint_matrix[i][k]->relations[j][l] = csp_bivalent->matrice_contraintes->constraint_matrix[i][k]->relations[j][l];
                        }
                    }
            }
        }

    return 1;
}

/***
 * Effectue les tests de consistence : SPC sur le sous csp binaire, AC sur tout le csp
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne 1 si le csp est SPC. 0 sinon
 ***/
int consistent(CSP * csp, int * affectation, int current_var, int niveau, int size){
    
    if(SPC(csp, affectation, current_var, niveau, size) == 0){
    	reload_relation(csp, current_var, niveau);
    	reload_domain(csp, current_var, 0, niveau);
    	printf("SPC echoue\n");
        return 0;
    }

    if(AC(csp, affectation, current_var, niveau) == 0){
    	reload_relation(csp, current_var, niveau);
    	reload_domain(csp, current_var, 0, niveau);
    	printf("AC echoue \n");
        return 0;
    }

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
int affecter(CSP * csp, int * affectation, int current_var, int size){

    if(size%2 == 0){
        if(affectation[current_var]+1 > (int)size/2)
            return 0;
    }else{
        if(affectation[current_var]+1 > (int)(size + 2 - 1)/2){ //arrondi vers le haut
            return 0;
        }
    }
    affectation[current_var]++;
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
        fprintf(glb_output_file,"solution : \n");
        for(int i=0; i < csp->max_var; i++)
            fprintf(glb_output_file,"x%d = %d \n",i, inst[i]);


        if(verify(csp2,inst))
            fprintf(glb_output_file,"BM : correct!\n");
        else
            fprintf(glb_output_file,"BM : Incorrect!\n");
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
    fprintf(glb_output_file,"\n*****************************BIGMAC***************************\n");
    CSP * bigmac_csp;
    int niveau = 0;
    int succes_consistence = 0;
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
    int current_var = niveau;
    while(not_complete(affectation, max_var) || !succes_consistence){
        //fprintf(glb_output_file,"niveau %d \n", niveau);
        current_var = niveau;
       
        if(affecter(csp, affectation, current_var, taille_domaine)){
            noeud_BM++;
             printf("test[%d] %d\n",niveau,affectation[niveau] ); 
            succes_consistence = consistent(csp, affectation, current_var, niveau, taille_domaine);
            if(succes_consistence){
                niveau++;
                printf("affectation[%d] %d\n",niveau-1,affectation[niveau-1] ); 
            }
            else{
                continue;
            }
        }
        else
        {
            printf("echec niveau %d\n", niveau);
            affectation[current_var] = 0;
            niveau--;
            reload_relation(csp, current_var-1, niveau);
            reload_domain(csp, current_var-1, 0, niveau);
            if(niveau < 0){
                free(affectation);
                fprintf(glb_output_file,"pas de solutions\n");
                return;
            }
        }
    }

    bigmac_csp = create_csp(csp, affectation, niveau-1, taille_domaine);

    print_matrix(bigmac_csp->Domain->domain_matrix,bigmac_csp->Domain->max_var,bigmac_csp->Domain->max_domain);
    solve_csp(bigmac_csp);


    free_csp(bigmac_csp);
    free(affectation);
}
