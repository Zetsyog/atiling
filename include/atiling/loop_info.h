#ifndef LOOP_INFO_H
#define LOOP_INFO_H

#include <osl/osl.h>
#include <stdio.h>

struct loop_info {
	size_t index;
	char *name;
	osl_names_p string_names;
	osl_relation_p relation;
	int start_row;
	int end_row;
	char *ub; /**< Upper bound */
	char *lb; /**< Lower bound */
};

typedef struct loop_info loop_info_t;
typedef struct loop_info *loop_info_p;

void loop_info_dump(FILE *, loop_info_p info);
void loop_info_free(loop_info_p info);
loop_info_p loop_info_get(osl_scop_p scop, size_t index);

size_t count_nested_loop(osl_scop_p scop);

#endif
