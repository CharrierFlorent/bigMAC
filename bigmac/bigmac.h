#ifndef H_BIGMAC
#define H_BIGMAC
#include "BT.h"
#include "structure.h"
#include "FC.h"
#include "AC8.h"
#include "RFL.h"
#include "PC8.h"


CSP * create_csp(CSP * csp, int * affectation, int niveau, int size);
int AC(CSP * csp, int * affectation, int current_var, int niveau);
int SPC(CSP * csp, int * affectation, int current_var, int niveau, int size);
int consistent(CSP * csp, int * affectation, int current_var, int niveau, int size);
int not_complete(int * array, int size);
int affecter(int * affectation, int current_var, int size);
void solve_csp(CSP * csp);
void bigmac(CSP *csp);
	

#endif
