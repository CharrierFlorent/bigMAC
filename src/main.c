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
#include "parser.h"
#include <unistd.h>
extern int noeud_BM;
extern int noeud_FC;
extern int noeud_BT;
extern int noeud_RFL;
int fcc = 0;
int rflc = 0;
int consistancy =  0;

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
    consistancy = 0;
    struct timeval st, et;
	gettimeofday(&st,NULL);
    int result = FC(csp, inst, var, 0, &st, &et);
    if(result != -1){
        for(int i = 0; i < csp->max_var;i++)
            fprintf(glb_output_file,"x%d = %d \n", i, inst[i]);

        if(verify(csp2,inst)){
            fprintf(glb_output_file,"FC : correct!\n");
            fcc++;
        }
        else
        	fprintf(glb_output_file,"FC : Incorrect!\n");
    }
    if(result == -1){
        fprintf(stdout,"noeud explores FC %d\n", noeud_FC);
        fprintf(stdout,"temps FC -1 microsecondes\n");  
        fprintf(stdout,"consistency check FC %d \n", consistancy);  
        free_csp(csp2);
        free(inst);
        free(var);

    }
    
    fprintf(stdout,"noeud explores FC %d\n", noeud_FC); 
    noeud_FC = 0;  

    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps FC %d microsecondes\n", elapsed);  
    fprintf(stdout,"consistency check FC %d \n", consistancy);  
	free_csp(csp2);
    free(inst);
    free(var);

}

void test_RFL(CSP * csp, HEURISTIQUE heuristique)
{
	noeud_RFL = 0;
	int * inst = calloc(csp->max_var,sizeof(int));
	CSP * csp2 = create_csp_by_copy(csp);
    consistancy = 0;
    struct timeval st, et;
	gettimeofday(&st,NULL);

	int result = RFL(csp, inst, heuristique, &st, &et);
    if(result == -1){
    	fprintf(stdout,"Timeout\n");  
        fprintf(stdout,"noeud explores RFL %d\n", noeud_RFL);
        fprintf(stdout,"temps RFL -1 microsecondes\n");  
        fprintf(stdout,"consistency check RFL %d \n", consistancy);  
        free_csp(csp2);
        free(inst);
        return;
    }

    if(verify(csp2,inst)){
    	fprintf(stdout,"Consistent\n");
        fprintf(glb_output_file,"RFL : correct!\n");
        rflc++;
    }
    else{
    	fprintf(glb_output_file,"RFL : Incorrect!\n");
    	fprintf(stdout,"Inconsistent\n");
    }

    fprintf(stdout,"noeud explores RFL %d\n", noeud_RFL);   


    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps RFL %d microsecondes\n", elapsed);  
    fprintf(stdout,"consistency check RFL %d \n", consistancy);  

    free_csp(csp2);
    free(inst);

}

void test_bigmac(CSP * csp, HEURISTIQUE heuristique){

    struct timeval st, et;
	gettimeofday(&st,NULL);
    consistancy = 0;
    noeud_BM = 0; 
    int result = bigmac(csp, heuristique, &st, &et);
    if(result == -1){
    	fprintf(stdout,"Timeout\n");
        fprintf(stdout,"noeud explores BM %d\n", noeud_BM); 
        fprintf(stdout,"temps BM -1 microsecondes\n");    
        fprintf(stdout,"consistency check BM %d\n", consistancy);
        return;
    }
    fprintf(stdout,"noeud explores BM %d\n", noeud_BM);   
    gettimeofday(&et,NULL);
    int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);   
    fprintf(stdout,"temps BM %d microsecondes\n", elapsed);  
    fprintf(stdout,"consistency check BM %d \n", consistancy);  
}


int usage(int argc, char *argv[]){

    int verbal = 0, file = 0;  
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i],"-v") == 0)
            verbal = 1;
        if(strcmp(argv[i],"-f") == 0)
            file = 1;

    }

    if(!verbal && !file && argc < 5){
        fprintf(stdout,"Usage : %s <nb variables> <taille domaine> <densite> <durete> <options>\n ", argv[0]);
        exit(0);
    }


    if(file && argc < 3){
        fprintf(stdout,"Usage avec fichier : %s -f <filename>\n ", argv[0]);
        exit(0);
    }

    if(!file && (atof(argv[3]) > 1 || atof(argv[4]) > 1 || atof(argv[3]) < 0 || atof(argv[4]) < 0)){
        fprintf(stdout,"Erreur argument, densite et durete doivent etre compris entre 0 et 1");
        exit(0);
    }

    if(verbal)
        glb_output_file = stdout;
    else
        glb_output_file = fopen("poubelle","w");

    return file;
    
}

int main(int argc, char *argv[]){
   	srand(rdtsc());
    int ar = 0;
    CSP * csp1; 
    FILE * f;
    while(ar < 1){
        if(usage(argc, argv)){
            f = fopen(argv[2], "r");
            printf("%s", argv[2]);
            csp1 = create_csp_from_file(f);
            printf(" %d\n", csp1->Domain->max_domain);
            fclose(f);
        }
        else
        	csp1 = generer_probleme(argv);
        
        CSP * csp2 = create_csp_by_copy(csp1);
        CSP * csp3 = create_csp_by_copy(csp1);
        CSP * csp4 = create_csp_by_copy(csp1);
      
        //test_BT    (csp1);
        //test_FC    (csp2);
        test_RFL   (csp1, MIN_DOMAINE);
        test_bigmac(csp2, MIN_DOMAINE);
		test_RFL   (csp3, DOMAINE_DEGRE);
        test_bigmac(csp4, DOMAINE_DEGRE);


        //test_AC_PC(csp1);
        //test_AC8(csp1);
        //test_PC8(csp1);
        free_csp(csp2);
        free_csp(csp3);
        free_csp(csp4);
        free_csp(csp1);
        ar++;
    }
    /*
    printf("rfl : %d\n", rflc);
    printf("bm : %d\n", bmc);
    printf("fc : %d\n", fcc);*/

    

    fclose(glb_output_file);
	return 0;
}
