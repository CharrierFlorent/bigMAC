/*generateur.c
 *Ce fichier contient les fonctions permettant de
 *generer des CSP aléatoire.
 */

#include "generateur.h"

/***
 * retourne la valeur absolue d'un entier
 * paramètre : - a : un entier
 * sortie : la valeur absolue d'un entier
 */
int valeur_absolue(int a){
    return (a > 0)? a : -a;
}

/***
 * genere un CSP aléatoire
 * paramètre : - n : le nombre de variable du csp
 *             - d : la taille du domaine des variable
 *             - p : la densité du graphe de contrainte du csp, vaut entre 0 et 1, à 1 le graphe est complet
 *             - t : la dureté des contraintes du csp, càd la proportion
 *                   de valeur autorisé pour une contrainte par rapport
 *                   au nombre total de couple possible. Vaut entre 0 et 1.
 * sortie : un csp
 */
CSP * generer_probleme(){
    srand(time(0));
    CSP * csp = malloc(sizeof(CSP));
    int nb_var, taille_domaine, m;
    double p, t;

	printf("Combien de variable ?\n");
	scanf("%d", &nb_var);
	printf("taille du domaine ?\n");
	scanf("%d", &taille_domaine);
    printf("Densité du graphe ? (entre 0 et 1)\n");
    scanf("%lf", &p);
    printf("Durete ? (entre 0 et 1)\n");
    scanf("%lf", &t);


    int cardinal_R = (1-t)*taille_domaine*taille_domaine;
    m = p*(nb_var)*(nb_var-1)/2;
    cardinal_R = valeur_absolue(cardinal_R);
    printf("construction d'un csp avec : N = %d, D = %d m  = %d, R = %d Densite = %f et durete = %f\n", nb_var, taille_domaine, m, cardinal_R, p, t);

    csp = init_empty_csp(nb_var, taille_domaine);
    printf("\n");
    int nb_contrainte = 0, nb_relations = 0;
    int var1 = 0, var2 = 0, d1 = 0, d2 = 0;
    while(nb_contrainte != m){
        do{
            var1 = rand()%nb_var;
            var2 = rand()%nb_var;
        }while(var1 == var2);
        if(!csp->matrice_contraintes->constraint_matrix[var1][var2]){
            csp->matrice_contraintes->constraint_matrix[var1][var2] = init_constraint(taille_domaine);
            csp->matrice_contraintes->constraint_matrix[var2][var1] = init_constraint(taille_domaine);

            while(nb_relations != cardinal_R){
                do{
                    d1 = rand()%taille_domaine;
                    d2 = rand()%taille_domaine;
                }while(csp->matrice_contraintes->constraint_matrix[var1][var2]->relations[d1][d2] == 1);
                csp->matrice_contraintes->constraint_matrix[var1][var2]->relations[d1][d2] = 1;
                csp->matrice_contraintes->constraint_matrix[var2][var1]->relations[d2][d1] = 1;
                nb_relations++;
            }
            nb_relations = 0;
            nb_contrainte++;
        }
    }
    return csp;
}
