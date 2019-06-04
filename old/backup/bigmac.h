#ifndef H_BIGMAC
#define H_BIGMAC
#include "BT.h"
#include "structure.h"
#include "AC8.h"
#include "PC8.h"

CSP * create_csp(CSP * csp, int * affectation, int niveau, int size);
int consistent(CSP * csp, int * affectation, int niveau, int size);
int not_complete(int * array, int size);
int affecter(int * affectation, int niveau, int size);
void solve_csp(CSP * csp);
int bigmac(CSP *csp);


#endif