#ifndef H_AC8
#define H_AC8
#include "structure.h"
#include "list.h"
#include <stddef.h>
#include <stdbool.h>

int first(int * array, int size);
int last(int * array, int size);
int next_value(int * array, int i, int size);
int withoutsupport_AC(int ** relations, int * domaine, int var_i, int var_j, int b, int size);
List * check_support(CSP * csp, List * list_ac, int * status_AC, int i, int j, int *var_status);
List * initialize_AC8(CSP * csp, int * status_AC, int *var_status);
void propagate_AC(CSP * csp, List * list_ac, int * status_AC, int i, int *var_status);
void AC8(CSP * csp, int *var_status);

#endif
