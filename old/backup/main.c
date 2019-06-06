#include<stdio.h>
#include<stdlib.h>
#include "bigmac.h"
#include "generateur.h"
#include "FC.h"
#include "BT.h"
#include "AC8.h"
#include "PC8.h"
#include "RFL.h"
#include <time.h>
#include <windows.h>
void v1(){
    LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
    double elapsedTime;
    clock_t t;
    double result_BT = 0.0, noeud_BT = 0.0, noeud_BM = 0.0, result_BM = 0.0;
    srand(time(0));
    double measure[120];
    int noeud[120];
    int nb_var = 30;
    //int domain = 8;
    double durete = 0.5;
    double densite = 0.5;

    int a = 0;
    double time_BT[20];
    double time_BM[20];
    double noeud_BM_arr[20];
    double noeud_BT_arr[20];

    for(int domain = 2; domain < 16; domain += 2){
        for(int counter  = 0; counter < 30; counter++){
            //printf("counter %d\n",counter);
            CSP * csp = generer_probleme_2(nb_var,domain,densite,durete);
            //CSP * csp2 = create_csp_by_copy(csp);
            //CSP * csp3 = create_csp_by_copy(csp);
            CSP * csp4 = create_csp_by_copy(csp);
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&t1);

            noeud[0+counter] = BackTrack(csp);

            QueryPerformanceCounter(&t2);
            elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
            measure[0+counter] = elapsedTime;
/*
            t = clock();
            Forward_Checking(csp2, MIN_DOMAINE);
            t = clock() - t;
            measure[30+counter] = ((double)t)/CLOCKS_PER_SEC; // in seconds

            t = clock();
            RFL(csp3,MIN_DOMAINE);
            t = clock() - t;
            measure[60+counter] = ((double)t)/CLOCKS_PER_SEC; // in seconds*/
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&t1);

            noeud[90+counter] = bigmac(csp4);

            QueryPerformanceCounter(&t2);
            elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
            measure[90+counter] = elapsedTime;
            free_csp(csp);
            //free_csp(csp2);
            //free_csp(csp3);
            free_csp(csp4);
        }
        for(int i = 0; i < 30; i++){
            result_BT += measure[0+i];
            //result_RFL += measure[60+i];
            result_BM += measure[90+i];
            noeud_BM += noeud[90+i];
            noeud_BT += noeud[0+i];
        }

        printf("\n n = %d, d = %d, densite = %f, tightness = %f\n",nb_var, domain, densite, durete);
        printf("Resultat : \n BT : %f noeud : %f, result_BM : %f noeud %f \n", result_BT/30.0, noeud_BT/30.0, result_BM/30.0, noeud_BM/30.0);
        time_BM[a] = result_BM/30.0;
        time_BT[a] = result_BT/30.0;
        noeud_BM_arr[a] = noeud_BM/30.0;
        noeud_BT_arr[a] = noeud_BT/30.0;

        result_BT = 0;
        result_BM =0;
        noeud_BM = 0;
        noeud_BT = 0;
        a++;
    }
    printf("a %d\n", a);
    printf("time_BM : {");
    for(int i = 0; i < a-1; i++)
        printf("%f, ", time_BM[i]);
    printf("%f}\n",time_BM[a-1]);

        printf("time_BT : {");
    for(int i = 0; i < a-1; i++)
        printf("%f, ", time_BT[i]);
    printf("%f}\n",time_BT[a-1]);

        printf("noeud_BM : {");
    for(int i = 0; i < a-1; i++)
        printf("%f, ", noeud_BM_arr[i]);
    printf("%f}\n",noeud_BM_arr[a-1]);

        printf("noeud_BT : {");
    for(int i = 0; i < a-1; i++)
        printf("%f, ", noeud_BT_arr[i]);
    printf("%f}\n",noeud_BT_arr[a-1]);
}

void v2(){
    clock_t t;
    double result_BT = 0.0, result_FC = 0.0, result_RFL = 0.0, result_BM = 0.0;
    srand(time(0));
    double measure[120];
    int nb_var = 128;
    int domain = 8;
    double densite = 0.4;
    for(double durete = 0.1; durete < 1.0; durete += 0.1){
        for(int counter  = 0; counter < 30; counter++){
            //printf("counter %d\n",counter);
            CSP * csp = generer_probleme_2(nb_var,domain,densite,durete);
            CSP * csp3 = create_csp_by_copy(csp);
            CSP * csp4 = create_csp_by_copy(csp);

            t = clock();
            BackTrack(csp);
            t = clock() - t;
            measure[0+counter] = ((double)t)/CLOCKS_PER_SEC; // in seconds

            printf("RFL\n");
            t = clock();
            RFL(csp3,MIN_DOMAINE);
            t = clock() - t;
            measure[60+counter] = ((double)t)/CLOCKS_PER_SEC; // in seconds
            printf("Fin \n");

            t = clock();
            bigmac(csp4);
            t = clock() - t;
            measure[90+counter] = ((double)t)/CLOCKS_PER_SEC; // in seconds

            free_csp(csp);
            free_csp(csp3);
            free_csp(csp4);
        }
        for(int i = 0; i < 30; i++){
            result_BT += measure[0+i];
            result_RFL += measure[60+i];
            result_BM += measure[90+i];
        }

        printf("\n n = %d, d = %d, densite = %f, tightness = %f\n",nb_var, domain, densite, durete);
        printf("Resultat : \n BT : %f, result_RFL : %f,  result_BM : %f \n", result_BT/30.0, result_RFL/30.0, result_BM/30.0);
        result_BT = 0;
        result_BM =0;
        result_FC = 0;
        result_RFL = 0;
    }
}

int main(){
    v1();
    /*
    CSP * csp = generer_probleme_2(30,40,0.6,0.6);
    RFL(csp, MIN_DOMAINE);*/
	return 0;
}
