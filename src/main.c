#include "atiling/atiling.h"
#include <osl/osl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	atiling_options_p options;
	FILE *input, *output;
	char **input_files = NULL;

	options = atiling_options_read(argc, argv, &input, &output);

	if (options->name != NULL) {
		atiling_fragment_p *fragment = atiling_extract(input, options);

		for (int i = 0; fragment[i] != NULL; i++) {
			atiling_fragment_idump(stdout, fragment[i], 0);
		}

		atiling_gen(input, output, fragment);

		for (int i = 0; fragment[i] != NULL; i++) {
			atiling_fragment_free(fragment[i]);
		};

		fclose(input);
	}

	// osl_scop_p scop;
	// // Read the OpenScop file.
	// scop = osl_scop_read(input);

	// printf("%li\n", count_nested_loop(scop));

	// loop_info_p info = loop_info_get(scop, 2);

	// loop_info_dump(info);

	// loop_info_free(info);

	// Dump the content of the scop data structure.
	// osl_scop_dump(stdout, scop);
	// Save the planet.

	// osl_scop_free(scop);
	atiling_options_free(options);

	return 0;
}
