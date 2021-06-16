#include "atiling/atiling.h"
#include <clan/clan.h>
#include <osl/osl.h>
#include <string.h>

void atiling_gen_pragma(FILE *, atiling_pragma_p);
void atiling_gen_iloop(FILE *, FILE *, atiling_pragma_p, osl_scop_p, int, int,
					   int);

/**
 * @param input
 * @param output
 * @param pragma_list
 */
void atiling_gen(FILE *input, FILE *output, atiling_pragma_p *pragma_list) {
	int pragma_index = 0;
	int line		 = 1;
	int column		 = 1;
	int ignore		 = ATILING_FALSE;
	int c;

	if (input == NULL)
		CLAN_error("null input");

	rewind(input);

	while ((c = fgetc(input)) != EOF) {
		column++;
		if (c == '\n') {
			line++;
			column = 1;
		}

		if (pragma_list[pragma_index] != NULL) {
			if (line == pragma_list[pragma_index]->start[0] &&
				column == pragma_list[pragma_index]->start[1]) {
				ignore = ATILING_TRUE;
				atiling_gen_pragma(output, pragma_list[pragma_index]);
			}
		}

		if (ignore == ATILING_FALSE) {
			fprintf(output, "%c", c);
		}

		if (pragma_list[pragma_index] != NULL) {
			if (ignore == ATILING_TRUE &&
				line == pragma_list[pragma_index]->end[0] &&
				column == pragma_list[pragma_index]->end[1]) {
				ignore = ATILING_FALSE;
				pragma_index++;
			}
		}
	}
}

void atiling_gen_pragma(FILE *output, atiling_pragma_p pragma) {
	fprintf(output, "\n");

	clan_options_p clan_opt = clan_options_malloc();
	ATILING_strdup(clan_opt->name, pragma->path);

	FILE *input = fopen(pragma->path, "r");
	if (input == NULL) {
		ATILING_error("cannot open pragma file");
	}

	ATILING_debug("extracting scop info with clan");
	osl_scop_p scop = clan_scop_extract(input, clan_opt);

	if (scop == NULL) {
		fprintf(stderr, "clan extract %s\n", clan_opt->name);
		ATILING_error("clan scop extract");
	}

	FILE *tmp = fopen("tmp.scop", "w");
	clan_scop_print(tmp, scop, clan_opt);

	int loop_count = count_nested_loop(scop);

	fprintf(output, "%s\n", "TODO: generate code vener");
	atiling_gen_iloop(input, output, pragma, scop, loop_count, 0, 0);

	clan_options_free(clan_opt);
	osl_scop_free(scop);
}

void atiling_gen_indent(FILE *file, int level) {
	for (int i = 0; i < level; i++) {
		fprintf(file, "\t");
	}
}

void atiling_gen_ipcmax(FILE *output, char *it_name, char **params, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "%s%s = %s%s(", it_name, ATILING_GEN_STR_PCMAX, it_name,
			ATILING_GEN_STR_EHRHART);

	for (int i = 0; params[i] != NULL; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", params[i]);
	}

	fprintf(output, ");\n");
}

void atiling_gen_ivol_level(FILE *output, int level, int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "%s%i = %s%s;", ATILING_GEN_STR_TILEVOL, level, "i",
			ATILING_GEN_STR_PCMAX);
}

void atiling_gen_iomp_pragma(FILE *output, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "#pragma omp parallel for firstprivate ");
}

void atiling_gen_iloop(FILE *input, FILE *output, atiling_pragma_p pragma,
					   osl_scop_p scop, int loop_count, int loop_index,
					   int ilevel) {
	loop_info_p info = loop_info_get(scop, loop_index);
	loop_info_dump(stderr, info);

	osl_strings_dump(stderr, info->string_names->parameters);

	atiling_gen_ipcmax(output, info->name,
					   info->string_names->parameters->string, ilevel);

	loop_info_free(info);
}
