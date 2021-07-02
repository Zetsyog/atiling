#include "atiling/atiling.h"

atiling_fragment_p atiling_fragment_malloc() {
	atiling_fragment_p frag;
	ATILING_malloc(frag, atiling_fragment_p, sizeof(atiling_fragment_t));
	frag->path		 = NULL;
	frag->scop		 = NULL;
	frag->start[0]	 = 0;
	frag->start[1]	 = 0;
	frag->end[0]	 = 0;
	frag->end[1]	 = 0;
	frag->divs		 = NULL;
	frag->loop_count = 0;
	frag->loops		 = NULL;
	return frag;
}

void atiling_fragment_free(atiling_fragment_p fragment) {
	for (int i = 0; i < fragment->loop_count; i++) {
		loop_info_free(fragment->loops[i]);
	}
	free(fragment->loops);
	for (int i = 0; fragment->divs[i] != NULL; i++) {
		free(fragment->divs[i]);
	}

	osl_scop_free(fragment->scop);
	free(fragment->path);
	free(fragment->divs);
	free(fragment);
}

void atiling_fragment_idump(FILE *file, atiling_fragment_p pragma, int level) {
	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}

	if (pragma != NULL) {
		fprintf(file, "+-- atiling_fragment_t \n");
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