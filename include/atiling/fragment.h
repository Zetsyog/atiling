#ifndef ATILING_FRAGMENT_H
#define ATILING_FRAGMENT_H

#include "atiling/loop_info.h"
#include "atiling/options.h"
#include <osl/osl.h>

struct atiling_fragment {
	char *path;			/**< File path containing extracted code */
	osl_scop_p scop;	/**< Osl scop generated with clan */
	int start[2];		/**< Start position in global file [line, col] */
	int end[2];			/**< End position [line, col] */
	char **divs;		/**< array of divisors ending by NULL */
	int loop_count;		/**< number of nested loop */
	loop_info_p *loops; /**< informations of each nested loop */
	int id;				/**< id of fragment (used when multiple pragmas) */
	atiling_options_p options;
};

typedef struct atiling_fragment atiling_fragment_t;
typedef struct atiling_fragment *atiling_fragment_p;

atiling_fragment_p atiling_fragment_malloc();
void atiling_fragment_free(atiling_fragment_p);

void atiling_fragment_idump(FILE *file, atiling_fragment_p pragma, int level);

#endif
