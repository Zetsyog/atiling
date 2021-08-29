#ifndef ATILING_OPTION_H
#define ATILING_OPTION_H

#include <stdio.h>

struct atiling_options {
	char *name;			/** Name of the input file */
	char *output;		/** Name of the output file */
	int keep_tmp_files; /** do not remove tmps files */
	int debug;
	int pluto_opt; /** Use pluto to perform more optimizations */
};

typedef struct atiling_options atiling_options_t;
typedef struct atiling_options *atiling_options_p;

/*+****************************************************************************
 *                        Structure display function                          *
 ******************************************************************************/
void atiling_options_print(FILE *, atiling_options_p);

/*+****************************************************************************
 *                    Memory allocation/deallocation function                 *
 ******************************************************************************/
atiling_options_p atiling_options_malloc();
void atiling_options_free(atiling_options_p);

/*+****************************************************************************
 *                            Reading function                                *
 ******************************************************************************/
atiling_options_p atiling_options_read(int, char **, FILE **, FILE **);

#endif
