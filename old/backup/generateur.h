#ifndef H_GENERATEUR
#define H_GENERATEUR

#include "structure.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

CSP * generer_probleme();
CSP * generer_probleme_2(int nb_var, int taille_domaine, double p, double t);
#endif
