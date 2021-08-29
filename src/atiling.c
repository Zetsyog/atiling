#include "atiling/atiling.h"
#include <clan/clan.h>
#include <string.h>

atiling_fragment_p atiling_parse(FILE *input, atiling_options_p options);

/**
 * @brief extract fragments of code from file
 * This function analyses code between pragmas 'trahrhre atiling' and store all
 * its useful data in a structure atiling_fragment_p
 * Then it returns an array all of extracted code
 * The returned array and the pointers it contains must be free'd.
 * @param[in] input 	The input file. Must be opened
 * @param[in] options	The global options
 * @return An array of code fragments
 */
atiling_fragment_p atiling_extract(FILE *input, atiling_options_p options) {
	atiling_fragment_p frag;

	frag		  = atiling_parse(input, options);
	frag->options = options;

	clan_options_p clan_opt = clan_options_malloc();

	ATILING_strdup(clan_opt->name, frag->path);

	ATILING_debug("Opening fragment file");
	FILE *frag_input = fopen(frag->path, "r");
	if (frag_input == NULL) {
		ATILING_error("cannot open fragment file");
	}

	ATILING_debug_call(fprintf(
		stdout, "[ATILING] Debug : extracting scop info with clan from %s\n",
		clan_opt->name));
	frag->scop = clan_scop_extract(frag_input, clan_opt);

	fclose(frag_input);

	if (frag->scop == NULL) {
		fprintf(stderr, "clan extract %s\n", clan_opt->name);
		ATILING_error("clan scop extract");
	}
	ATILING_debug("done");

	ATILING_debug("extracting loop info");
	frag->loop_count = atiling_count_nested_loop(frag->scop);

	frag->loops = malloc(sizeof(atiling_loop_p) * frag->loop_count);

	for (int j = 0; j < frag->loop_count; j++) {
		frag->loops[j] = atiling_loop_info_get(frag->scop, j);
	}

	atiling_fragment_get_loops_bound(frag);

	ATILING_debug("extracting loop info: done");

	if (frag->loop_count < frag->div_len) {
		ATILING_error("Too much divisors for not enough nested loops");
	}

	if (frag->loop_count > frag->div_len) {
		ATILING_warning("Not enough divisors; Inner loops will not be tiled");
		int missing_divs = frag->loop_count - frag->div_len;
		ATILING_realloc(frag->divs, sizeof(char *) * (frag->loop_count + 1));
		for (int i = 0; i < missing_divs; i++) {
			ATILING_strdup(frag->divs[frag->div_len + i], "1");
		}
		frag->divs[frag->loop_count] = NULL;

		frag->div_len = frag->loop_count;
	}

	clan_options_free(clan_opt);

	return frag;
}