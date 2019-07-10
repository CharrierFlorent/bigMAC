#ifndef H_BIGRFL
#define H_BIGRFL
#include "BT.h"
#include "structure.h"
#include "FC.h"
#include "AC8.h"
#include "RFL.h"
#include "PC8.h"
#include <sys/time.h>

void reload_relations(CSP * csp, int niveau);
void reduire_relations(CSP * csp, int * affectation, int current_var, int niveau);
void reduire_domaine(CSP * csp, int * affectation, int current_var, int niveau);
int relation_is_empty(CSP * csp, int a, int b);
int check_pc_consistence(CSP* csp);
int AC(CSP * csp, int * affectation, int current_var, int niveau);
int SPC(CSP * csp, int * affectation, int * ordre_affectation, int current_var, int niveau, int size);
int consistent(CSP * csp, int * affectation, int * ordre_affectation, int current_var, int niveau, int size);
int not_in_domain(CSP * csp, int * affectation, int current_var);
int affecter(CSP * csp, int * affectation, int current_var, int size);
int bigmac(CSP *csp, HEURISTIQUE heuristique, struct timeval * st, struct timeval * et);
#endif