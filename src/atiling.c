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
	atiling_fragment_p frag = atiling_parse(input, options);
	frag->options			= options;

	clan_options_p clan_opt = clan_options_malloc();

	ATILING_strdup(clan_opt->name, frag->path);
	ATILING_debug("dup");

	ATILING_debug("Opening fragment file");
	FILE *frag_input = fopen(frag->path, "r");
	if (frag_input == NULL) {
		ATILING_error("cannot open fragment file");
	}

	ATILING_debug_call(fprintf(
		stdout, "[ATILING] Debug : extracting scop info with clan from %s\n",
		clan_opt->name));
	frag->scop = clan_scop_extract(frag_input, clan_opt);

	ATILING_debug("well");

	fclose(frag_input);

	if (frag->scop == NULL) {
		fprintf(stderr, "clan extract %s\n", clan_opt->name);
		ATILING_error("clan scop extract");
	}
	ATILING_debug("done");

	ATILING_debug("extracting loop info");
	frag->loop_count = atiling_count_nested_loop(frag->scop);
	ATILING_debug("loop count done");

	frag->loops = malloc(sizeof(loop_info_p) * frag->loop_count);

	for (int j = 0; j < frag->loop_count; j++) {
		frag->loops[j] = atiling_loop_info_get(frag->scop, j);
	}
	ATILING_debug("extracting loop info: done");

	clan_options_free(clan_opt);

	return frag;
}