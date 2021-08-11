#include "atiling/atiling.h"
#include <osl/osl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	atiling_options_p options;
	FILE *input, *output;
	char **input_files = NULL;

	options = atiling_options_read(argc, argv, &input, &output);

	ATILING_debug("Writing temp info files");

	// write .srcfilename info file
	FILE *fileinfo = fopen(".srcfilename", "w");
	if (fileinfo == NULL) {
		exit(1);
	}
	fprintf(fileinfo, "%s", options->name);
	fclose(fileinfo);

	// write .outfilename info file
	fileinfo = fopen(".outfilename", "w");
	if (fileinfo == NULL) {
		exit(1);
	}
	fprintf(fileinfo, "%s", options->output);
	fclose(fileinfo);

	if (options->name != NULL) {

		// extract code fragments
		ATILING_debug("Extracting fragments");
		atiling_fragment_p fragment = atiling_extract(input, options);
		atiling_fragment_idump(stdout, fragment, 0);

		if (options->pluto_opt) {
			atiling_apply_transform(fragment);
		}

		FILE *tmp = fopen("tmp.scop", "w");
		osl_scop_print(tmp, fragment->scop);
		fclose(tmp);

		// write new code in output file
		ATILING_debug("Generating new code");
		atiling_gen(input, output, fragment, options);

		// save the planet
		atiling_fragment_free(fragment);

		fclose(input);
	}

	// still saving the planet
	atiling_options_free(options);

	return 0;
}
