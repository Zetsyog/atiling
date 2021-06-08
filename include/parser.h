#ifndef PARSER_H
#define PARSER_H

#include <osl/osl.h>
#include <stdio.h>

struct loop_info {
	size_t index;
	char *name;
	int precision;
	int start_len;
	osl_int_p start;
	int end_len;
	osl_int_p end;
};

typedef struct loop_info loop_info_t;
typedef struct loop_info *loop_info_p;

size_t count_nested_loop(osl_scop_p scop);

loop_info_p loop_info_get(osl_scop_p scop, size_t index);

void loop_info_dump(loop_info_p info);
void loop_info_free(loop_info_p info);

#endif