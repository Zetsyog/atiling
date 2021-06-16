#ifndef ATILING_PRAGMA_H
#define ATILING_PRAGMA_H

#include <osl/osl.h>

struct atiling_pragma {
	char *path;		 /**< File path containing extracted code */
	osl_scop_p scop; /**< Osl scop of loops */
	int start[2];	 /**< Start position in global file [line, col] */
	int end[2];		 /**< End position [line, col] */
	char **divs;	 /**< array of divisors ending by NULL */
};

typedef struct atiling_pragma atiling_pragma_t;
typedef struct atiling_pragma *atiling_pragma_p;

atiling_pragma_p atiling_pragma_malloc();
void atiling_pragma_free(atiling_pragma_p);

void atiling_pragma_idump(FILE *file, atiling_pragma_p pragma, int level);

#endif
