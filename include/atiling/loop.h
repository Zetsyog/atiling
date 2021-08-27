#ifndef ATILING_LOOP_H
#define ATILING_LOOP_H

#include <osl/osl.h>
#include <stdio.h>

struct atiling_loop {
	char **name_array;
	osl_relation_p domain;
	char *ub; /**< Upper bound */
	char *lb; /**< Lower bound */
	char *it;
	size_t depth;
	int start_row;
	int end_row;
	osl_strings_p parameters_names;
};

typedef struct atiling_loop atiling_loop_t;
typedef struct atiling_loop *atiling_loop_p;

void atiling_loop_info_dump(FILE *, atiling_loop_p info);
void atiling_loop_info_free(atiling_loop_p info);
void loop_info_names_get(atiling_loop_p);
atiling_loop_p atiling_loop_info_get(osl_scop_p scop, size_t index);

size_t atiling_count_nested_loop(osl_scop_p scop);

#endif
