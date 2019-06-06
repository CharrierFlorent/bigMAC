#include <stdio.h>
#include <stdlib.h>
#include "bigmac.h"
#include "generateur.h"
#include "FC.h"
#include "BT.h"
#include "AC8.h"
#include "PC8.h"
#include "RFL.h"

extern int noeud_BM;
extern int noeud_FC;
extern int noeud_BT;


void test_BT(CSP * csp){
	CSP * csp2 = create_csp_by_copy(csp);
	CSP * csp3 = create_csp_by_copy(csp);
    printf("########### BT ################\n");
    int * inst = calloc(csp->max_var,sizeof(int));

    if(BT(csp2,inst, 0)){
        printf("solution : \n");
        for(int i=0; i < csp2->max_var; i++)
            for (int j=0; j<csp2->Domain->max_domain; j++)
                if(csp2->Domain->domain_matrix[i][j] == 1)
                    printf("x%d = %d \n",i, j);

        if(verify(csp3,inst))
            printf("BT : correct!\n");
        else
        	printf("BT : Incorrect!\n");
    }
    free_csp(csp2);
    free_csp(csp3);
    free(inst);
    printf("noeud explores BT %d\n", noeud_BT);   
}

void test_FC(CSP * csp){
    printf("FC\n");
    int * inst = calloc(csp->max_var,sizeof(int));
    int * var = calloc(csp->max_var,sizeof(int));
    CSP * csp2 = create_csp_by_copy(csp);
    //Forward_Checking(csp, inst, 42);
    if(FC(csp, inst, var, 0)){
        for(int i = 0; i < csp->max_var;i++)
            printf("x%d = %d \n", i, inst[i]);

        if(verify(csp2,inst))
            printf("FC : correct!\n");
        else
        	printf("FC : Incorrect!\n");
        }
    free_csp(csp2);
    free(inst);
    free(var);
    printf("noeud explores FC %d\n", noeud_FC);   
}

void test_RFL (CSP * csp)
{
	int * inst = calloc(csp->max_var,sizeof(int));
	CSP * csp2 = create_csp_by_copy(csp);
	RFL (csp, inst, MIN_DOMAINE);
	if(verify(csp2,inst))
        printf("RFL : correct!\n");
    else
    	printf("RFL : Incorrect!\n");
    free_csp(csp2);
    free(inst);
}

void test_bigmac(CSP * csp){
    bigmac(csp);
    printf("noeud explores BM %d\n", noeud_BM);    
}


void test_PC8(CSP * csp){
    //test_BT(csp);
    printf("Domaine avant PC8\n");
    //print_relation(csp);
    printf("#########################################################################################################\n");
    PC8(csp);
    printf("Domaine après PC8\n");
    //print_relation(csp);
    test_BT(csp);
}

void test_AC8(CSP * csp){

        printf("Domaine avant AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        AC8(csp,1, NULL);

        printf("Domaine après AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        //test_BT(csp);
}

void test_AC_PC(CSP * csp){
    AC8(csp, 1, NULL);
    PC8(csp);
    test_BT(csp);
}

int main(){
        srand(time(0));

//    while(1){
        CSP * csp1 = generer_probleme();
        print_csp(csp1);
        
        CSP * csp2 = create_csp_by_copy(csp1);
        CSP * csp3 = create_csp_by_copy(csp1);
        CSP * csp4 = create_csp_by_copy(csp1);
        
        test_BT    (csp1);
        test_FC    (csp2);
        test_RFL   (csp3);
        test_bigmac(csp4);


        //test_AC_PC(csp1);
        //test_AC8(csp1);
        //test_PC8(csp1);
        free_csp(csp2);
        free_csp(csp3);
        free_csp(csp4);
        free_csp(csp1);
    //}
	return 0;
}
