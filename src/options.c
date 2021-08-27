#include "atiling/atiling.h"
#include <getopt.h>
#include <string.h>

/*+****************************************************************************
 *                         Memory allocation function                         *
 ******************************************************************************/

/**
 * @brief Allocate a new clan_options_t
 * This functions allocate the memory space for a clan_options_t structure and
 * fill its fields with the defaults values. It returns a pointer to the
 * allocated clan_options_t structure.
 * @return The pointer to the newly allocated atiling_options_t
 */
atiling_options_p atiling_options_malloc(void) {
	atiling_options_p options;

	// Memory allocation for the clan_options_t structure.
	ATILING_malloc(options, sizeof(atiling_options_t));

	// We set the various fields with default values.
	options->name			= NULL; // Name of the input file is not set.
	options->output			= "stdout";
	options->keep_tmp_files = ATILING_FALSE;
	options->pluto_opt		= ATILING_FALSE;
	return options;
}

/**
 * @brief Free memory of atiling_options_t
 * This function frees the allocated memory for a atiling_options_t structure.
 * @param options Option structure to be freed.
 */
void atiling_options_free(atiling_options_p options) {
	free(options->name);
	free(options);
}

/*+****************************************************************************
 *                            Processing functions                            *
 ******************************************************************************/
/**
 * This function displays the quick help when the user set the option -help
 * while calling clan. Prints are cut to respect the 509 characters
 * limitation of the ISO C 89 compilers.
 */
void atiling_options_help() {
	printf("Usage: atiling [ options ] input_file\n");
	printf(
		"\nGeneral options:\n"
		"  -o <output>          Name of the output file; 'stdout' is a "
		"special\n"
		"  -v, --version        Display the release information (and more).\n"
		"  -h, --help           Display this information.\n\n");
	printf(
		"The special value 'stdin' for 'input_file' or the special option '-' "
		"makes atiling\n"
		"to read data on standard input.\n\n");
}

/**
 * @brief Convert str output value to FILE *
 * The value "stdout" is a special value that will point to stdout
 * @param[in] 		value
 * @param[in, out]	output
 */
void atiling_options_set_output(char *value, FILE **output,
								atiling_options_p options) {
	if (*output != stdout) {
		fclose(*output);
		ATILING_error("output must be unique");
	}
	// stdout is a special value to set output to standard output.
	if (strcmp(value, "stdout") == 0) {
		*output = stdout;
	} else {
		*output = fopen(value, "w");
		if (*output == NULL)
			ATILING_error("cannot open the output file");
	}
	ATILING_strdup(options->output, value);
}

/**
 * This functions reads all the options and the input/output files thanks
 * the the user's calling line elements (in argc). It fills a atiling_options_t
 * structure.
 * @param argc[in]    Number of strings in command line.
 * @param argv[in]    Array of command line strings.
 * @param input[out]  Input file
 * @param output[out]  Output file
 */
atiling_options_p atiling_options_read(int argc, char **argv, FILE **input,
									   FILE **output) {
	int infos				  = ATILING_FALSE;
	atiling_options_p options = atiling_options_malloc();
	*output					  = stdout;
	*input					  = NULL;

	int c;
	int digit_optind = 0;

	static struct option long_options[] = {{"version", no_argument, 0, 0},
										   {"help", no_argument, 0, 0},
										   {"output", required_argument, 0, 0},
										   {"verbose", no_argument, 0, 0},
										   {"transform", no_argument, 0, 0},
										   {"notrahrhe", no_argument, 0, 0},
										   {0, 0, 0, 0}};

	while (1) {
		int this_option_optind = optind ? optind : 1;
		int option_index	   = 0;
		c = getopt_long(argc, argv, "hvo:", long_options, &option_index);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			// Print version
			if (!strcmp(long_options[option_index].name, "version")) {
				fprintf(stderr, "version %s\n", ATILING_VERSION);
				infos = ATILING_TRUE;
			}
			// Print usage
			else if (!strcmp(long_options[option_index].name, "help")) {
				atiling_options_help();
				infos = ATILING_TRUE;
			}
			// Print debug info
			else if (!strcmp(long_options[option_index].name, "verbose")) {
				// TODO
			}
			// Do pluto optimization
			else if (!strcmp(long_options[option_index].name, "transform")) {
				options->pluto_opt = ATILING_TRUE;
			}
			// Set output file
			else if (!strcmp(long_options[option_index].name, "output")) {
				atiling_options_set_output(optarg, output, options);
			}
			break;
		case 'h':
			// Print usage
			atiling_options_help();
			infos = ATILING_TRUE;
			break;
		case 'v':
			// Print version
			fprintf(stderr, "version %s\n", ATILING_VERSION);
			infos = ATILING_TRUE;
			break;
		case 'o':
			// Set output file
			atiling_options_set_output(optarg, output, options);
			break;
		}
	}

	// One arg (input file) is required if we do not print info on program (like
	// --version or --help)
	if (infos == ATILING_FALSE) {
		if (optind > argc - 1) {
			ATILING_error("usage: not enough arg (-h for help)");
		} else if (optind < argc - 1) {
			ATILING_error("usage: too many args (-h for help)");
		} else {
			// '-' as input arg is special char that will read input in stdin
			if (!strcmp(argv[optind], "-")) {
				printf("j%s\n", argv[optind]);
				*input = stdin;
				ATILING_strdup(options->name, "stdin");

			} else {
				*input = fopen(argv[optind], "r");
				if (*input == NULL)
					ATILING_error("cannot open input file");

				ATILING_strdup(options->name, argv[optind]);
			}
		}
	}

	if (*input == NULL && infos == ATILING_FALSE)
		ATILING_error("no input file (-h for help)");

	return options;
}