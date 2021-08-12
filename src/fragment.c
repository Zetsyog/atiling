#include "atiling/atiling.h"
#include "string.h"

atiling_fragment_p atiling_fragment_malloc() {
	atiling_fragment_p frag;
	ATILING_malloc(frag, atiling_fragment_p, sizeof(atiling_fragment_t));
	frag->path		 = NULL;
	frag->scop		 = NULL;
	frag->start[0]	 = 0;
	frag->start[1]	 = 0;
	frag->end[0]	 = 0;
	frag->end[1]	 = 0;
	frag->div_len	 = 0;
	frag->divs		 = NULL;
	frag->loop_count = 0;
	frag->loops		 = NULL;
	frag->id		 = 0;
	frag->options	 = NULL;
	return frag;
}

void atiling_fragment_free(atiling_fragment_p fragment) {
	for (int i = 0; i < fragment->loop_count; i++) {
		atiling_loop_info_free(fragment->loops[i]);
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

/**
 * @param[in] fragment
 * @param[in] depth
 * @return whether the loop of given depth must be tiled or not
 */
int is_tiling_enabled(atiling_fragment_p fragment, int depth) {
	if (depth < fragment->loop_count) {
		if (fragment->divs[depth][0] == '1' && fragment->divs[depth][1] == 0) {
			return ATILING_FALSE;
		}
	}
	return ATILING_TRUE;
}
