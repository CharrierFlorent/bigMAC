#include "bigRFL.h"

extern FILE * glb_output_file;
int noeud_BM = 0;
int bmc = 0;

void fill_constraint(CSP * csp){
    int taille_dom = csp->Domain->max_domain; 
 	for(int i = 0; i < csp->max_var; i++){
    	for(int j = 0; j < csp->max_var; j++)
	        if(i != j && csp->matrice_contraintes->constraint_matrix[i][j] == NULL){
	        	csp->matrice_contraintes->constraint_matrix[i][j] = init_constraint(taille_dom);
	            for(int l = 0; l < taille_dom; l++){
	                for(int k = 0; k < taille_dom; k++){
	                        csp->matrice_contraintes->constraint_matrix[i][j]->relations[l][k] = 1;
	                }
	            }
	        }    
	}
}

void reload_relations(CSP * csp, int niveau){
    int taille_dom = csp->Domain->max_domain; 
    for(int i = 0; i < csp->max_var; i++)
    	for(int l = 0; l < csp->max_var; l++)
	        if(csp->matrice_contraintes->constraint_matrix[l][i]){
	            for(int j = 0; j < taille_dom; j++){
	                for(int k = 0; k < taille_dom; k++){
	                    if(csp->matrice_contraintes->constraint_matrix[l][i]->relations[j][k] == -(niveau+OFFSET)){
	                        csp->matrice_contraintes->constraint_matrix[l][i]->relations[j][k] = 1;
	                        csp->matrice_contraintes->constraint_matrix[i][l]->relations[k][j] = 1;
	                    }
	                }
	            }
	        }    
}

void reduire_relations(CSP * csp, int * affectation, int current_var, int niveau){
    int taille_dom = csp->Domain->max_domain;
    int e1, e2;
    int element_1 = 2*(affectation[current_var]-1);
    int element_2 = (element_1 == csp->Domain->max_domain-1) ? element_1 : element_1+1;  
    for(int i = 0; i < csp->max_var; i++){
    	if(affectation[i] > 0){
    		e1 = 2*(affectation[i]-1); e2 = (e1 == csp->Domain->max_domain-1) ? e1 : e1+1; 
    	}
    	else{
    		e1 = -1;
    		e2 = -1;
    	}
        if(csp->matrice_contraintes->constraint_matrix[current_var][i]){
            for(int j = 0; j < taille_dom; j++){
                for(int k = 0; k < taille_dom; k++){
                    if((j != element_1 && j != element_2) && (k != e1 && k != e2) && csp->matrice_contraintes->constraint_matrix[current_var][i]->relations[j][k] == 1){
                        csp->matrice_contraintes->constraint_matrix[current_var][i]->relations[j][k] = -(niveau+OFFSET);
                        csp->matrice_contraintes->constraint_matrix[i][current_var]->relations[k][j] = -(niveau+OFFSET);
                    }
                }
            }
        }
    }
}

void reduire_domaine(CSP * csp, int * affectation, int current_var, int niveau){

    int element_1 = 2*(affectation[current_var]-1);
    int element_2 = (element_1 == csp->Domain->max_domain-1) ? element_1 : element_1+1;     
    for(int d = 0; d < csp->Domain->max_domain; d++)
        if(d != element_1 && d!= element_2 && csp->Domain->domain_matrix[current_var][d] == 1){
            csp->Domain->domain_matrix[current_var][d] = NOT_AFFECTED;
            csp->Domain->taille_domaine[current_var]--;
        }   
  /*  reduce_domain(csp, current_var, element_1);
    csp->Domain->domain_matrix[current_var][element_2] = 1;
    csp->Domain->taille_domaine[current_var]++;*/
}


int relation_is_empty(CSP * csp, int a, int b){
	int taille_dom = csp->Domain->max_domain; 
    for(int j = 0; j < taille_dom; j++){
        for(int k = 0; k < taille_dom; k++){
            if(csp->matrice_contraintes->constraint_matrix[a][b]->relations[j][k] == 1 && csp->Domain->domain_matrix[a][j] == 1 && csp->Domain->domain_matrix[b][k] == 1 ){
            	return 0;
            }
        }
    }
    return 1;
}

int check_pc_consistence(CSP* csp){
	int taille_dom = csp->Domain->max_domain; 
    for(int i = 0; i < csp->max_var; i++)
    	for(int j = 0; j < csp->max_var; j++)
        	if(csp->matrice_contraintes->constraint_matrix[i][j]){
        		if(relation_is_empty(csp, i, j)){
        			return 0;
        		}
        	}
    return 1;
         
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

void copy_csp_to_bivalent_v2(CSP * csp, CSP * csp_bivalent, int * affectation, int * ordre_affectation, int niveau){
	int i_1,i_2,j_1,j_2, var_i, var_j;
	for(int i = 0; i < 	niveau; i++){
		var_i = ordre_affectation[i];
		for(int j = 0; j < niveau; j++){
			var_j = ordre_affectation[j];
			if(i != j && csp->matrice_contraintes->constraint_matrix[var_i][var_j]){
				i_1 = 2*(affectation[var_i]-1);
				i_2 = (i_1 == csp->Domain->max_domain-1) ? i_1 : i_1+1;
				j_1 = 2*(affectation[var_j]-1);
				j_2 = (j_1 == csp->Domain->max_domain-1) ? j_1 : j_1+1;
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][0] = csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_1][j_1];
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][1] = csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_1][j_2];
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][0] = csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_2][j_1];
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][1] = csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_2][j_2];
				csp_bivalent->Domain->taille_domaine[i] = csp->Domain->taille_domaine[var_i];
				csp_bivalent->Domain->taille_domaine[j] = csp->Domain->taille_domaine[var_j];
				csp_bivalent->Domain->domain_matrix[i][0] = csp->Domain->domain_matrix[var_i][i_1];
				csp_bivalent->Domain->domain_matrix[i][1] = csp->Domain->domain_matrix[var_i][i_2];
				csp_bivalent->Domain->domain_matrix[j][0] = csp->Domain->domain_matrix[var_j][j_1];
				csp_bivalent->Domain->domain_matrix[j][1] = csp->Domain->domain_matrix[var_j][j_2];
			}
		}
	}
}


void copy_csp_to_bivalent(CSP * csp, CSP * csp_bivalent, int * affectation){
	int i_1,i_2,j_1,j_2;
	for(int i = 0; i < csp_bivalent->max_var; i++)
		for(int j = 0; j < csp_bivalent->max_var; j++){
			if(i != j && csp->matrice_contraintes->constraint_matrix[i][j]){
				i_1 = 2*(affectation[i]-1);
				i_2 = (i_1 == csp->Domain->max_domain-1) ? i_1 : i_1+1;
				j_1 = 2*(affectation[j]-1);
				j_2 = (j_1 == csp->Domain->max_domain-1) ? j_1 : j_1+1;
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][0] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_1][j_1];
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][1] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_1][j_2];
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][0] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_2][j_1];
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][1] = csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_2][j_2];
				csp_bivalent->Domain->taille_domaine[i] = csp->Domain->taille_domaine[i];
				csp_bivalent->Domain->taille_domaine[j] = csp->Domain->taille_domaine[j];
				csp_bivalent->Domain->domain_matrix[i][0] = csp->Domain->domain_matrix[i][i_1];
				csp_bivalent->Domain->domain_matrix[i][1] = csp->Domain->domain_matrix[i][i_2];
				csp_bivalent->Domain->domain_matrix[j][0] = csp->Domain->domain_matrix[j][j_1];
				csp_bivalent->Domain->domain_matrix[j][1] = csp->Domain->domain_matrix[j][j_2];
			}
		}
}


void copy_bivalent_to_csp_v2(CSP * csp, CSP * csp_bivalent, int * affectation, int * ordre_affectation, int niveau){
	int i_1,i_2,j_1,j_2, var_i, var_j;
    for(int i = 0; i < niveau; i++){
    	var_i = ordre_affectation[i];
		for(int j = 0; j < niveau; j++){
			var_j = ordre_affectation[j];
			if(csp->matrice_contraintes->constraint_matrix[var_i][var_j]){
				i_1 = 2*(affectation[var_i]-1);
				i_2 = (i_1 == csp->Domain->max_domain-1) ? i_1 : i_1+1;
				j_1 = 2*(affectation[var_j]-1);
				j_2 = (j_1 == csp->Domain->max_domain-1) ? j_1 : j_1+1;
				csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_1][j_1] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][0];
				csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_1][j_2] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][1];
				csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_2][j_1] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][0];
				csp->matrice_contraintes->constraint_matrix[var_i][var_j]->relations[i_2][j_2] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][1];
				csp->Domain->taille_domaine[var_i] = csp_bivalent->Domain->taille_domaine[i];
				csp->Domain->taille_domaine[var_j] = csp_bivalent->Domain->taille_domaine[j];
				csp->Domain->domain_matrix[var_i][i_1] = csp_bivalent->Domain->domain_matrix[i][0];
				csp->Domain->domain_matrix[var_i][i_2] = csp_bivalent->Domain->domain_matrix[i][1];
				csp->Domain->domain_matrix[var_j][j_1] = csp_bivalent->Domain->domain_matrix[j][0];
				csp->Domain->domain_matrix[var_j][j_2] = csp_bivalent->Domain->domain_matrix[j][1];
			}
		}
	}
}


void copy_bivalent_to_csp(CSP * csp, CSP * csp_bivalent, int * affectation){
	int i_1,i_2,j_1,j_2;
    for(int i = 0; i < csp_bivalent->max_var; i++)
		for(int j = 0; j < csp_bivalent->max_var; j++){
			if(csp->matrice_contraintes->constraint_matrix[i][j]){
				i_1 = 2*(affectation[i]-1);
				i_2 = (i_1 == csp->Domain->max_domain-1) ? i_1 : i_1+1;
				j_1 = 2*(affectation[j]-1);
				j_2 = (j_1 == csp->Domain->max_domain-1) ? j_1 : j_1+1;
				csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_1][j_1] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][0];
				csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_1][j_2] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[0][1];
				csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_2][j_1] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][0];
				csp->matrice_contraintes->constraint_matrix[i][j]->relations[i_2][j_2] = csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[1][1];
				csp->Domain->taille_domaine[i] = csp_bivalent->Domain->taille_domaine[i];
				csp->Domain->taille_domaine[j] = csp_bivalent->Domain->taille_domaine[j];
				csp->Domain->domain_matrix[i][i_1] = csp_bivalent->Domain->domain_matrix[i][0];
				csp->Domain->domain_matrix[i][i_2] = csp_bivalent->Domain->domain_matrix[i][1];
				csp->Domain->domain_matrix[j][j_1] = csp_bivalent->Domain->domain_matrix[j][0];
				csp->Domain->domain_matrix[j][j_2] = csp_bivalent->Domain->domain_matrix[j][1];
			}
		}
}
/***
 * Test de SPC. Crée un sous csp binaire, puis execute AC8 et PC8 dessus
 * paramètre : - csp : un csp
 *             - affectation : l'affectation courante de toute les variables
 *             - niveau : la profondeur dans l'arbre de recherche
 *             - size : taille du domaine
 * sortie : retourne 1 si le csp est SPC. 0 sinon
 ***/
int SPC(CSP * csp, int * affectation, int * ordre_affectation, int current_var, int niveau, int size){
	CSP * csp_bivalent = init_empty_csp(niveau+1, 2);
	for(int i = 0; i < csp_bivalent->max_var; i++)
		for(int j = 0; j < csp_bivalent->max_var; j++){
			if(i != j){
				csp_bivalent->matrice_contraintes->constraint_matrix[i][j] = init_constraint(2);
				for(int k = 0; k < 2; k++)
					for(int l = 0; l < 2; l++)
						csp_bivalent->matrice_contraintes->constraint_matrix[i][j]->relations[k][l] = 1;
			}
		}

	copy_csp_to_bivalent_v2(csp, csp_bivalent, affectation, ordre_affectation, niveau+1);
    //copy_csp_to_bivalent(csp, csp_bivalent, affectation);
    PC8(csp_bivalent,OFFSET+niveau);    
    if(check_pc_consistence(csp_bivalent) == 0){
    	free_csp(csp_bivalent);
        return 0;
    }


    AC8(csp_bivalent,OFFSET+niveau,NULL);
    if(cherche_domaine_vide(csp_bivalent->Domain->taille_domaine, csp_bivalent->Domain->max_var, NULL)){
        free_csp(csp_bivalent);
        return 0;
    }

	copy_bivalent_to_csp_v2(csp,csp_bivalent, affectation, ordre_affectation, niveau+1);
    //copy_bivalent_to_csp(csp,csp_bivalent, affectation);

    free_csp(csp_bivalent);
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
int consistent(CSP * csp, int * affectation, int * ordre_affectation, int current_var, int niveau, int size){
    
    if(SPC(csp, affectation, ordre_affectation, current_var, niveau, size) == 0){
    	//printf("SPC echoue\n");
        return 0;
    }


    AC8(csp, OFFSET+niveau,NULL);
    if(cherche_domaine_vide(csp->Domain->taille_domaine, csp->Domain->max_var, NULL)){
    	//printf("echec AC8\n");
        return 0;
    }
    return 1;
}

int not_in_domain(CSP * csp, int * affectation, int current_var){

	int element_1 = 2*(affectation[current_var]-1);
    int element_2 = (element_1 == csp->Domain->max_domain-1) ? element_1 : element_1+1;
    if(csp->Domain->domain_matrix[current_var][element_1] != 1 && csp->Domain->domain_matrix[current_var][element_2] !=1)
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
void solve_csp(CSP * csp, CSP* csp2, struct timeval * st, struct timeval * et){
    int * inst = calloc(csp->max_var,sizeof(int));	
    int * var = calloc(csp->max_var,sizeof(int));

    if(FC(csp, inst, var, 0, st, et)){
        fprintf(glb_output_file,"solution : \n");
        for(int i=0; i < csp->max_var; i++)
            fprintf(glb_output_file,"x%d = %d \n",i, inst[i]);


        if(verify(csp2,inst)){
            fprintf(glb_output_file,"BM : correct!\n");
            bmc++;
        }
        else
            fprintf(glb_output_file,"BM : Incorrect!\n");
    }
    free(inst);
    free(var);
}


int not_complete(int * array, int size){
    for(int i =0; i < size; i++)
        if(array[i] == 0)
            return 1;
    return 0;
}



/***
 * Execute l'algorithme du bigMAC sur un csp donné
 * paramètre : - csp : un csp
 ***/
int bigmac(CSP *csp,HEURISTIQUE heuristique, struct timeval * st, struct timeval * et){
    fprintf(glb_output_file,"\n*****************************BIGMAC***************************\n");
    CSP * csp2 = create_csp_by_copy(csp);
    int niveau = 0;
    int succes_consistence = 0;
    int succes_affectation = 0;
    int max_var = csp->max_var;
    int taille_domaine = csp->Domain->max_domain;
    int * affectation; /* Un tableau d'affectation, pour l'instant on ne va pas implementer d'heuristique de choix de pair de variables
                        * On a donc que chaque variables a pour domaine taille_domaine/2 valeur possible.
                        * ainsi si elle vaut 1, alors on considerera le couple des 2 premiere valeurs, si elle vaut 3 le 3eme couple
                        * de valeur etc... 0 signifie non affecté.
                        */
	int * var_status = malloc(csp->max_var*sizeof(int));
    int * ordre_affectation = malloc(csp->max_var*sizeof(int));
    affectation = malloc(csp->max_var*sizeof(int));
    for(int i = 0; i < max_var; i++){affectation[i] = 0; ordre_affectation[i] = 0; var_status[i] = 0;}

    /* Algo : On affecte une valeur. Si ce n'est pas possible (domaine etudié en entier
     * on arrete, pas de solutions.
     * On teste la double consistence sur le sous csp ensuite. Si ce n'est pas consistent
     * on test la valeur suivante, s'il n'y a plus de valeur suivante possible, on remonte d'un cran
     * On s'arrete quand on a explorer tout l'arbre
     */
    int current_var;
    int elapsed;
    
    while(not_complete(var_status, max_var) || !succes_consistence){
    	//timeout
		gettimeofday(et,NULL);
        elapsed = ((et->tv_sec - st->tv_sec) * 1000000) + (et->tv_usec - st->tv_usec);  
        if(elapsed > 120000000)
            return -1;  
        //choix variable
    	if(not_complete(var_status, max_var)){
	        current_var = choix_variable(csp, heuristique, var_status, niveau);
	        var_status[current_var] = 0;
	        ordre_affectation[niveau] = current_var;
	    }

		succes_affectation = affecter(csp, affectation, current_var, taille_domaine);
    	if(succes_affectation && not_in_domain(csp,affectation,current_var))
    		continue;
        
        if(succes_affectation){
        	noeud_BM++;
	    	var_status[current_var] = 1;
            reduire_domaine(csp, affectation, current_var, niveau);
           	reduire_relations(csp, affectation, current_var, niveau);

            succes_consistence = consistent(csp, affectation, ordre_affectation, current_var, niveau, taille_domaine);
            
            if(succes_consistence){
            	var_status[current_var] = 1;
                niveau++;
            }
            else{	
            	var_status[current_var] = 0;
                reload_relations(csp, niveau);
                reload_domain(csp, current_var, 0,niveau);
            }
        }
        else{
            affectation[current_var] = 0;
            var_status[current_var] = 0;

		    niveau--;
         	if(niveau < 0){
            	free(ordre_affectation);
            	free(var_status);
                free(affectation);
                fprintf(glb_output_file,"pas de solutions\n");
                return 0;
            }
            reload_relations(csp, niveau);
            reload_domain(csp, ordre_affectation[niveau], 0, niveau);
            var_status[ordre_affectation[niveau]] = 0;
        }
    }

    //CSP * bigmac_csp = create_csp(csp, affectation, csp->max_var-1, taille_domaine);
    /*for(int i = 0; i < csp->max_var;i++)
    	reduire_domaine(csp, affectation, i, 0);*/
    //print_matrix(bigmac_csp->Domain->domain_matrix,bigmac_csp->Domain->max_var,bigmac_csp->Domain->max_domain);
    solve_csp(csp, csp2, st, et);
    free_csp(csp2);
    //free_csp(bigmac_csp);
    free(affectation);
    free(ordre_affectation);
    return 1;
}
