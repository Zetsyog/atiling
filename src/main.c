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
		// extract code fragments
		atiling_fragment_p *fragment = atiling_extract(input, options);

		for (int i = 0; fragment[i] != NULL; i++) {
			atiling_fragment_idump(stdout, fragment[i], 0);
		}

		// write new code in output file
		atiling_gen(input, output, fragment);

		// save the planet
		for (int i = 0; fragment[i] != NULL; i++) {
			atiling_fragment_free(fragment[i]);
		};

		fclose(input);
	}

	// still saving the planet
	atiling_options_free(options);

	return 0;
}
