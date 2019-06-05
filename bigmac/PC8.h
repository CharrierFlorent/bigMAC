#ifndef H_PC8
#define H_PC8
#include "structure.h"
#include "list.h"
#include <stddef.h>
#include <stdbool.h>
#include "AC8.h"


int withoutsupport_PC(Constraint_mat * contraintes, int * domaine_k, int var_i, int var_j, int var_k, int a, int b, int size);

List * check_support_PC(CSP * csp, List * list_PC, int dim1, int dim2, int dim3, int status_PC[][dim2][dim3], int i, int j, int a, int b);

List * initialize_PC8(CSP * csp, int dim1, int dim2, int dim3, int status_PC[][dim2][dim3]);

List * propagate_PC(CSP * csp, List * list_PC, int dim1, int dim2, int dim3, int status_PC[][dim2][dim3], int i, int k, int a);

void PC8(CSP * csp);

#endif
