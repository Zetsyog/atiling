#include "atiling/atiling.h"

atiling_pragma_p atiling_pragma_malloc() {
	atiling_pragma_p pragma;
	ATILING_malloc(pragma, atiling_pragma_p, sizeof(atiling_pragma_t));
	pragma->path	 = NULL;
	pragma->scop	 = NULL;
	pragma->start[0] = 0;
	pragma->start[1] = 0;
	pragma->end[0]	 = 0;
	pragma->end[1]	 = 0;
	ATILING_malloc(pragma->divs, char **, sizeof(char *));
	pragma->divs[0] = NULL;
	return pragma;
}

void atiling_pragma_free(atiling_pragma_p pragma) {
	free(pragma->path);
	free(pragma->divs);
	free(pragma);
}

void atiling_pragma_idump(FILE *file, atiling_pragma_p pragma, int level) {
	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}

	if (pragma != NULL) {
		fprintf(file, "+-- atiling_pragma_t \n");
	} else {
		fprintf(file, "+-- NULL pragma \n");
	}

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   start: %i,%i\n", pragma->start[0], pragma->start[1]);

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   end: %i,%i\n", pragma->end[0], pragma->end[1]);

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   divs:\n");

	for (int i = 0; pragma->divs[i] != NULL; i++) {
		// indent
		for (int i = 0; i < level; i++) {
			fprintf(file, "|\t");
		}
		fprintf(file, "|\t|   %s\n", pragma->divs[i]);
	}
}