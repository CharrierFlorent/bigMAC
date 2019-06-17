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
extern int noeud_RFL;
FILE * glb_output_file;

void test_BT(CSP * csp){
	CSP * csp2 = create_csp_by_copy(csp);
	CSP * csp3 = create_csp_by_copy(csp);
    fprintf(glb_output_file,"*************************************** BT ***************************************\n");
    int * inst = calloc(csp->max_var,sizeof(int));

    if(BT(csp2,inst, 0)){
        fprintf(glb_output_file,"solution : \n");
        for(int i=0; i < csp2->max_var; i++)
            for (int j=0; j<csp2->Domain->max_domain; j++)
                if(csp2->Domain->domain_matrix[i][j] == 1)
                    fprintf(glb_output_file,"x%d = %d \n",i, j);

        if(verify(csp3,inst))
            fprintf(glb_output_file,"BT : correct!\n");
        else
        	fprintf(glb_output_file,"BT : Incorrect!\n");
    }
    free_csp(csp2);
    free_csp(csp3);
    free(inst);
    fprintf(stdout,"noeud explores BT %d\n", noeud_BT);   
    noeud_BT = 0;
}

void test_FC(CSP * csp){
    fprintf(glb_output_file,"*************************************** FC ***************************************\n");
    int * inst = calloc(csp->max_var,sizeof(int));
    int * var = calloc(csp->max_var,sizeof(int));
    CSP * csp2 = create_csp_by_copy(csp);
    //Forward_Checking(csp, inst, 42);
    if(FC(csp, inst, var, 0)){
        for(int i = 0; i < csp->max_var;i++)
            fprintf(glb_output_file,"x%d = %d \n", i, inst[i]);

        if(verify(csp2,inst))
            fprintf(glb_output_file,"FC : correct!\n");
        else
        	fprintf(glb_output_file,"FC : Incorrect!\n");
        }
    free_csp(csp2);
    free(inst);
    free(var);
    fprintf(stdout,"noeud explores FC %d\n", noeud_FC); 
    noeud_FC = 0;  
}

void test_RFL (CSP * csp)
{
	int * inst = calloc(csp->max_var,sizeof(int));
	CSP * csp2 = create_csp_by_copy(csp);
	RFL (csp, inst, 42);
	if(verify(csp2,inst))
        fprintf(glb_output_file,"RFL : correct!\n");
    else
    	fprintf(glb_output_file,"RFL : Incorrect!\n");
    free_csp(csp2);
    free(inst);
    fprintf(stdout,"noeud explores RFL %d\n", noeud_RFL);   
    noeud_RFL = 0;
}

void test_bigmac(CSP * csp){
    bigmac(csp);
    fprintf(stdout,"noeud explores BM %d\n", noeud_BM);  
    noeud_BM = 0;  
}


void test_PC8(CSP * csp){
    //test_BT(csp);
    fprintf(glb_output_file,"Domaine avant PC8\n");
    //print_relation(csp);
    fprintf(glb_output_file,"#########################################################################################################\n");
    PC8(csp);
    fprintf(glb_output_file,"Domaine après PC8\n");
    //print_relation(csp);
    test_BT(csp);
}

void test_AC8(CSP * csp){

        fprintf(glb_output_file,"Domaine avant AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        AC8(csp,1, NULL);

        fprintf(glb_output_file,"Domaine après AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        //test_BT(csp);
}

void test_AC_PC(CSP * csp){
    AC8(csp, 1, NULL);
    PC8(csp);
    test_BT(csp);
}

int main(int argc, char *argv[]){
    srand(time(0));

    
    if(argc < 5){
        fprintf(stdout,"Usage : %s <nb variables> <taille domaine> <densite> <durete>\n", argv[0]);
        exit(0);
    }

    if(atof(argv[3]) > 1 || atof(argv[4]) > 1 || atof(argv[3]) < 0 || atof(argv[4]) < 0){
        fprintf(stdout,"Erreur argument, densite et durete doivent etre compris entre 0 et 1");
        exit(0);
    }

    if(argc > 5 && strcmp(argv[5],"-v") == 0)
        glb_output_file = stdout;
    else
        glb_output_file = fopen("poubelle","w");

    CSP * csp1 = generer_probleme(argv);
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
    fclose(glb_output_file);
	return 0;
}
