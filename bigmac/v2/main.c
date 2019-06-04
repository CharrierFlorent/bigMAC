#include <stdio.h>
#include <stdlib.h>
#include "generateur.h"

//#include <windows.h>

int verify(CSP * csp, int * solution){
    for(int i = 0; i < csp->max_var; i++)
        for(int j = 0; j < csp->max_var; j++){
                if(csp->constraint_matrix[i][j]){
                    if(!csp->constraint_matrix[i][j]->relations[solution[i]][solution[j]])
                        return 0;
                }
        }
    return 1;
}

int main(){


    //while(1){
        CSP * csp = generer_probleme();
        free_csp(csp);
    //}
	return 0;
}
