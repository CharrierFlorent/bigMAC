#include "parser.h"

CSP * create_csp_from_file(FILE * fp){
	CSP * csp;
    char * line = NULL;
    size_t len = 0;
    size_t read;
    int taille_domaine = 0, nb_var = 0;
    //fp = fopen("queen5-5", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE); 
    char * pch;
    char * old;
    int i = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);
        if(line[0] == 'd'){
        	nb_var++;
        	pch = strtok (line," ");
		    while (pch != NULL)
			{
				old = pch;
				pch = strtok (NULL, " ,.-");
			}
			sscanf(old,"%c%d", &pch, &i);
			if(i > taille_domaine)
				taille_domaine = i;
        }
        if(line[0] == 'x')
        	break;	
    }
    int val1, val2, var1, var2;
    char * la;
    char * lo;
	printf("nb_var %d\ntaille dom : %d\n",nb_var, taille_domaine);
    csp = init_empty_csp(nb_var, taille_domaine);
    rewind(fp);
    while ((read = getline(&line, &len, fp)) != -1) {

    	if(line[0] == 'x'){
			sscanf(line, "%c%d %c%d", &la, &var1, &lo, &var2);
            csp->matrice_contraintes->constraint_matrix[var1-1][var2-1] = init_constraint(taille_domaine);
            csp->matrice_contraintes->constraint_matrix[var2-1][var1-1] = init_constraint(taille_domaine);
    	}
        if(line[0] == 'v'){
        	sscanf(line, "%c%d %c%d", &la, &val1, &lo, &val2);
            csp->matrice_contraintes->constraint_matrix[var1-1][var2-1]->relations[val1-1][val2-1] = 1;
            csp->matrice_contraintes->constraint_matrix[var2-1][var1-1]->relations[val2-1][val1-1] = 1;
        }
    }

	return csp;

}