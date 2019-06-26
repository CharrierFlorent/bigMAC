#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "bigRFL.h"
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
int fcc = 0;
int rflc = 0;
extern int bmc;
FILE * glb_output_file;

unsigned long long rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((unsigned long long)hi << 32) | lo;
}

void test_BT(CSP * csp){
	CSP * csp2 = create_csp_by_copy(csp);
	CSP * csp3 = create_csp_by_copy(csp);
    fprintf(glb_output_file,"*************************************** BT ***************************************\n");
    int * inst = calloc(csp->max_var,sizeof(int));

    struct timeval st, et;
	gettimeofday(&st,NULL);
    
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
	gettimeofday(&et,NULL);
    fprintf(stdout,"noeud explores BT %d\n", noeud_BT);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps BT %d microsecondes\n", elapsed);   
    
    noeud_BT = 0;

    free_csp(csp2);
    free_csp(csp3);
    free(inst);

}

void test_FC(CSP * csp){
    fprintf(glb_output_file,"*************************************** FC ***************************************\n");
    int * inst = calloc(csp->max_var,sizeof(int));
    int * var = calloc(csp->max_var,sizeof(int));
    CSP * csp2 = create_csp_by_copy(csp);
    
    struct timeval st, et;
	gettimeofday(&st,NULL);

    if(FC(csp, inst, var, 0)){
        for(int i = 0; i < csp->max_var;i++)
            fprintf(glb_output_file,"x%d = %d \n", i, inst[i]);

        if(verify(csp2,inst)){
            fprintf(glb_output_file,"FC : correct!\n");
            fcc++;
        }
        else
        	fprintf(glb_output_file,"FC : Incorrect!\n");
        }
    
    fprintf(stdout,"noeud explores FC %d\n", noeud_FC); 
    noeud_FC = 0;  

    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps FC %d microsecondes\n", elapsed);  

	free_csp(csp2);
    free(inst);
    free(var);

}

void test_RFL (CSP * csp)
{
	int * inst = calloc(csp->max_var,sizeof(int));
	CSP * csp2 = create_csp_by_copy(csp);

    struct timeval st, et;
	gettimeofday(&st,NULL);

	RFL (csp, inst, 42);
	if(verify(csp2,inst)){
        fprintf(glb_output_file,"RFL : correct!\n");
        rflc++;
    }
    else
    	fprintf(glb_output_file,"RFL : Incorrect!\n");

    fprintf(stdout,"noeud explores RFL %d\n", noeud_RFL);   
    noeud_RFL = 0;

    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps RFL %d microsecondes\n", elapsed);  

    free_csp(csp2);
    free(inst);

}

void test_bigmac(CSP * csp){
    struct timeval st, et;
	gettimeofday(&st,NULL);

    bigmac(csp);
    fprintf(stdout,"noeud explores BM %d\n", noeud_BM);  
    noeud_BM = 0;  


    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps BM %d microsecondes\n", elapsed);  
}

/*
void test_PC8(CSP * csp){
    //test_BT(csp);
    fprintf(glb_output_file,"Domaine avant PC8\n");
    print_relation(csp);
    fprintf(glb_output_file,"#########################################################################################################\n");
csp->max_var = 4;
        
    PC8(csp,3);
csp->max_var = 10;
        
    fprintf(glb_output_file,"Domaine après PC8\n");
    print_relation(csp);
    //test_BT(csp);
}

void test_AC8(CSP * csp){

        fprintf(glb_output_file,"Domaine avant AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        csp->max_var = 4;
        AC8(csp,1, NULL);
        csp->max_var = 10;
        

        fprintf(glb_output_file,"Domaine après AC8\n");
        print_matrix(csp->Domain->domain_matrix,csp->Domain->max_var,csp->Domain->max_domain);
        //test_BT(csp);
}

void test_AC_PC(CSP * csp){
    AC8(csp, 1, NULL);
    PC8(csp);
    test_BT(csp);
}*/

int main(int argc, char *argv[]){
    srand(rdtsc());
	
 
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
int ar = 0;
while(ar < 1){
    CSP * csp1 = generer_probleme(argv);
    //print_csp(csp1);
    
    CSP * csp2 = create_csp_by_copy(csp1);
    CSP * csp3 = create_csp_by_copy(csp1);
    CSP * csp4 = create_csp_by_copy(csp1);
  
    //test_BT    (csp1);
    //test_FC    (csp2);
    test_RFL   (csp3);
    test_bigmac(csp4);


    //test_AC_PC(csp1);
    //test_AC8(csp1);
    //test_PC8(csp1);
    free_csp(csp2);
    free_csp(csp3);
    free_csp(csp4);
    ar++;
    free_csp(csp1);
}
    /*printf("rfl : %d\n", rflc);
    printf("bm : %d\n", bmc);
    printf("fc : %d\n", fcc);*/

    fclose(glb_output_file);
	return 0;
}
