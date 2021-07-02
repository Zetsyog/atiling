#include "atiling/atiling.h"
#include <clan/clan.h>
#include <string.h>

atiling_fragment_p *atiling_parse(FILE *input, atiling_options_p options);

atiling_fragment_p *atiling_extract(FILE *input, atiling_options_p options) {
	atiling_fragment_p *frags = atiling_parse(input, options);

	for (int i = 0; frags[i] != NULL; i++) {
		clan_options_p clan_opt = clan_options_malloc();
		ATILING_strdup(clan_opt->name, frags[i]->path);

		FILE *input = fopen(frags[i]->path, "r");
		if (input == NULL) {
			ATILING_error("cannot open fragment file");
		}

		ATILING_debug("extracting scop info with clan");
		frags[i]->scop = clan_scop_extract(input, clan_opt);

		fclose(input);

		if (frags[i]->scop == NULL) {
			fprintf(stderr, "clan extract %s\n", clan_opt->name);
			ATILING_error("clan scop extract");
		}
		ATILING_debug("done");

		ATILING_debug("extracting loop info");
		frags[i]->loop_count = count_nested_loop(frags[i]->scop);
		ATILING_debug("loop count done");

		frags[i]->loops = malloc(sizeof(loop_info_p) * frags[i]->loop_count);

		for (int j = 0; j < frags[i]->loop_count; j++) {
			frags[i]->loops[j] = loop_info_get(frags[i]->scop, j);
		}
		ATILING_debug("extracting loop info: done");

		FILE *tmp = fopen("tmp.scop", "w");
		clan_scop_print(tmp, frags[i]->scop, clan_opt);
		fclose(tmp);

		clan_options_free(clan_opt);
	}

	return frags;
}