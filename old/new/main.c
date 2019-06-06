#include<stdio.h>
#include<stdlib.h>
#include "bigmac.h"
#include "generateur.h"
#include "FC.h"
#include "BT.h"
#include "AC8.h"
#include "PC8.h"
#include "RFL.h"
//#include <windows.h>

void test_BT(CSP * csp){
    BackTrack(csp);
}

void test_FC(CSP * csp){
    Forward_Checking(csp, MIN_DOMAINE);
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
	RFL (csp, MIN_DOMAINE);
}

int main(){


    while(1){
        CSP * csp = generer_probleme();
        CSP * csp2 = create_csp_by_copy(csp);
        //test_AC8(csp);
        //test_PC8(csp);
        //test_FC(csp);
        test_BT(csp2);
        //test_AC_PC(csp);
        //test_RFL (csp2);
        bigmac(csp);
        free_csp(csp);
        free_csp(csp2);
    }
	return 0;
}
