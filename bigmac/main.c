#include <stdio.h>
#include <stdlib.h>
#include "bigmac.h"
#include "generateur.h"
#include "FC.h"
#include "BT.h"
#include "AC8.h"
#include "PC8.h"
#include "RFL.h"
//#include <windows.h>

int verify(CSP * csp, int * solution){
    for(int i = 0; i < csp->max_var; i++)
        for(int j = 0; j < csp->max_var; j++){
                if(csp->matrice_contraintes->constraint_matrix[i][j]){
                    if(!csp->matrice_contraintes->constraint_matrix[i][j]->relations[solution[i]][solution[j]])
                        return 0;
                }
        }
    return 1;
}

void test_BT(CSP * csp){
	CSP * csp2 = create_csp_by_copy(csp);
	CSP * csp3 = create_csp_by_copy(csp);
    /*printf("########### V1 ################\n");
    BackTrack(csp);*/
    printf("########### V2 ################\n");
    int * inst = calloc(csp->max_var,sizeof(int));

    if(BT(csp2,inst, 0)){
        printf("solution : \n");
        for(int i=0; i < csp2->max_var; i++)
            for (int j=0; j<csp2->Domain->max_domain; j++)
                if(csp2->Domain->domain_matrix[i][j] == 1)
                    printf("x%d = %d \n",i, j);

        if(verify(csp3,inst))
            printf("BT : correct!\n");
    }
    free_csp(csp2);
    free_csp(csp3);
}

void test_FC(CSP * csp){
    int * inst = calloc(csp->max_var,sizeof(int));
    CSP * csp2 = create_csp_by_copy(csp);
    Forward_Checking(csp,inst, MIN_DOMAINE);
    if(verify(csp2,inst))
        printf("FC : correct!\n");
    free_csp(csp2);
}

void test_PC8(CSP * csp){
    //test_BT(csp);
    printf("Domaine avant PC8\n");
    //print_relation(csp);
    printf("#########################################################################################################\n");
    printf("#########################################################################################################\n");
    printf("#########################################################################################################\n");
    PC8(csp);
    printf("Domaine après PC8\n");
    //print_relation(csp);
    test_BT(csp);
}

void test_AC8(CSP * csp){

        printf("Domaine avant AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        AC8(csp, NULL);

        printf("Domaine après AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        //test_BT(csp);
}

void test_AC_PC(CSP * csp){
    AC8(csp, NULL);
    PC8(csp);
    test_BT(csp);
}

void test_RFL (CSP * csp)
{
	int * inst = calloc(csp->max_var,sizeof(int));
	CSP * csp2 = create_csp_by_copy(csp);
	RFL (csp, inst, MIN_DOMAINE);
	if(verify(csp2,inst))
        printf("RFL : correct!\n");
    free_csp(csp2);
}

int main(){


    while(1){
        CSP * csp = generer_probleme();
        CSP * csp2 = create_csp_by_copy(csp);
        CSP * csp3 = create_csp_by_copy(csp);
        //test_AC8(csp);
        //test_PC8(csp);
        //test_BT(csp);

        test_FC(csp2);
        //test_AC_PC(csp);
        test_RFL (csp3);
        bigmac(csp);
        free_csp(csp);
        free_csp(csp2);
        free_csp(csp3);
    }
	return 0;
}
