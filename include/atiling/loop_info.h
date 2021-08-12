#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include <osl/osl.h>
#include <stdio.h>

struct loop_info {
	char *name;
	char **name_array;
	osl_relation_p domain;
	char *ub; /**< Upper bound */
	char *lb; /**< Lower bound */
	size_t index;
	int start_row;
	int end_row;
	osl_strings_p parameters_names;
};

typedef struct loop_info loop_info_t;
typedef struct loop_info *loop_info_p;

void atiling_loop_info_dump(FILE *, loop_info_p info);
void atiling_loop_info_free(loop_info_p info);
void loop_info_names_get(loop_info_p);
loop_info_p atiling_loop_info_get(osl_scop_p scop, size_t index);
void atiling_loop_info_bound_print(FILE *file, loop_info_p info, int row,
								   char *);

size_t atiling_count_nested_loop(osl_scop_p scop);

#endif
