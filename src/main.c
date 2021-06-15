#include "atiling/atiling.h"
#include <osl/osl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 2) {
		fprintf(stderr, "Error: usage: %s [file] \n", argv[0]);
		return 1;
	}

	FILE *input = fopen(argv[1], "r");

	osl_scop_p scop;
	// Read the OpenScop file.
	scop = osl_scop_read(input);

	printf("%li\n", count_nested_loop(scop));

	loop_info_p info = loop_info_get(scop, 2);

	loop_info_dump(info);

	loop_info_free(info);

	// Dump the content of the scop data structure.
	// osl_scop_dump(stdout, scop);
	// Save the planet.
	osl_scop_free(scop);

	fclose(input);

	return 0;
}