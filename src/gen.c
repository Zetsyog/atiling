#include "atiling/atiling.h"
#include <clan/clan.h>
#include <osl/osl.h>
#include <string.h>

void atiling_gen_fragment(FILE *, atiling_fragment_p);
void atiling_gen_iloop(FILE *, atiling_fragment_p, int, osl_strings_p, int);

/**
 * @param input
 * @param output
 * @param fragment_list
 */
void atiling_gen(FILE *input, FILE *output, atiling_fragment_p *fragment_list) {
	int fragment_index = 0;
	int line		   = 1;
	int column		   = 1;
	int ignore		   = ATILING_FALSE;
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

		if (fragment_list[fragment_index] != NULL) {
			if (line == fragment_list[fragment_index]->start[0] &&
				column == fragment_list[fragment_index]->start[1]) {
				ignore = ATILING_TRUE;
				atiling_gen_fragment(output, fragment_list[fragment_index]);
			}
		}

		if (ignore == ATILING_FALSE) {
			fprintf(output, "%c", c);
		}

		if (fragment_list[fragment_index] != NULL) {
			if (ignore == ATILING_TRUE &&
				line == fragment_list[fragment_index]->end[0] &&
				column == fragment_list[fragment_index]->end[1]) {
				ignore = ATILING_FALSE;
				fragment_index++;
			}
		}
	}
}

void atiling_gen_fragment(FILE *output, atiling_fragment_p fragment) {
	fprintf(output, "\n");

	fprintf(output, "\n// begin atiling\n");
	atiling_gen_iloop(output, fragment, 0, NULL, 0);
	fprintf(output, "// end atiling\n");
}

void atiling_gen_indent(FILE *file, int level) {
	for (int i = 0; i < level; i++) {
		fprintf(file, "\t");
	}
}

void atiling_gen_ixpcmax(FILE *output, char *it_name, char **params,
						 int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "long int %s%s = %s%s(", it_name, ATILING_GEN_STR_PCMAX,
			it_name, ATILING_GEN_STR_EHRHART);

	for (int i = 0; params[i] != NULL; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", params[i]);
	}

	fprintf(output, ");\n");
}

void atiling_gen_ivol_level(FILE *output, int level, char *it_name, char *div,
							int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "long int %s%i = %s%s / %s;\n", ATILING_GEN_STR_TILEVOL,
			level, it_name, ATILING_GEN_STR_PCMAX, div);
}

void atiling_gen_iubxt(FILE *output, char *x, int vol_level, int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "long int ub%st = max(%s%s / (%s%i) - 1, 0);\n", x, x,
			ATILING_GEN_STR_PCMAX, ATILING_GEN_STR_TILEVOL, vol_level);
}

void atiling_gen_iomp_pragma(FILE *output, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "#pragma omp parallel\n");
}

void atiling_gen_ifor_begin(FILE *output, char *x, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "for(long int %st = 0; %st <= ub%st; %st++) {\n", x, x, x,
			x);
}

void atiling_gen_ifor_end(FILE *output, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "}\n");
}

void atiling_gen_ilbx(FILE *output, char *x, char **params, int level,
					  int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "long int lb%s = %s%s(max(%st * (%s%i + 1), 1), ", x,
			ATILING_GEN_STR_TRAHRHE, x, x, ATILING_GEN_STR_TILEVOL, level);

	for (int i = 0; params[i] != NULL; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", params[i]);
	}
	fprintf(output, ");\n");
}

void atiling_gen_iubx(FILE *output, char *x, char **params, int level,
					  int ilevel) {
	atiling_gen_indent(output, ilevel);

	// trahrhe i(min(( it+1)∗(TILE VOL L1+1),i pcmax),N, M) − 1;
	fprintf(output, "long int ub%s = %s%s(min((%st + 1) * (%s%i + 1), %s%s), ",
			x, ATILING_GEN_STR_TRAHRHE, x, x, ATILING_GEN_STR_TILEVOL, level, x,
			ATILING_GEN_STR_PCMAX);

	for (int i = 0; params[i] != NULL; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", params[i]);
	}
	fprintf(output, ");\n");
}

void atiling_gen_iinner_loop(FILE *output, atiling_fragment_p fragment,
							 int ilevel) {
	for (int i = 0; i < fragment->loop_count; i++) {
		atiling_gen_indent(output, ilevel + 1 + i);
		fprintf(output, "// TODO for\n");
		char *x = fragment->loops[i]->name;
		atiling_gen_indent(output, ilevel + 1 + i);

		if (fragment->divs[i][0] == 0 && fragment->divs[i][1] == 0) {
			fprintf(output, "// for normal\n");
		} else {
			fprintf(output,
					"for(%s = max(0, lb%s); %s < min(TODO, ); %s++) {\n", x, x,
					x, x);
		}

		osl_statement_p statement = fragment->scop->statement;
		osl_body_p body =
			osl_generic_lookup(fragment->scop->extension, OSL_URI_BODY);

		while (statement != NULL) {
			osl_generic_p st_ext = statement->extension;
			osl_body_p body		 = st_ext->data;

			if (body != NULL) {
				if (osl_strings_size(body->iterators) == i + 1) {
					osl_body_p ext =
						osl_generic_lookup(statement->extension, OSL_URI_BODY);

					if (ext != NULL) {
						atiling_gen_indent(output, ilevel + 2 + i);
						osl_strings_print(output, ext->expression);
					}
				}
			}

			statement = statement->next;
		}
	}
	for (int i = fragment->loop_count - 1; i >= 0; i--) {
		atiling_gen_indent(output, ilevel + 1 + i);
		fprintf(output, "}\n");
	}
}

void atiling_gen_iloop(FILE *output, atiling_fragment_p fragment,
					   int loop_index, osl_strings_p params, int ilevel) {
	// stop condition
	if (loop_index >= fragment->loop_count)
		return;

	osl_scop_p scop	 = fragment->scop;
	loop_info_p info = fragment->loops[loop_index];

	if (!(fragment->divs[loop_index][0] == 0 &&
		  fragment->divs[loop_index][1] == 0)) {
		// loop_info_dump(stderr, info);

		// osl_strings_dump(stderr, info->string_names->parameters);

		if (params == NULL) {
			params		   = osl_strings_malloc();
			unsigned int i = 0;
			while (info->string_names->parameters->string[i] != NULL) {
				osl_strings_add(params,
								info->string_names->parameters->string[i]);
				i++;
			}
		}

		// osl_strings_dump(stdout, params);

		atiling_gen_ixpcmax(output, info->name, params->string, ilevel);
		atiling_gen_ivol_level(output, loop_index + 1, info->name,
							   fragment->divs[loop_index], ilevel);
		atiling_gen_iubxt(output, info->name, loop_index + 1, ilevel);
		fprintf(output, "\n");

		if (loop_index == 0) {
			atiling_gen_iomp_pragma(output, ilevel);
		}
		atiling_gen_ifor_begin(output, info->name, ilevel);

		atiling_gen_ilbx(output, info->name, params->string, loop_index + 1,
						 ilevel + 1);
		atiling_gen_iubx(output, info->name, params->string, loop_index + 1,
						 ilevel + 1);

		fprintf(output, "\n");

		// Append lbx and ubx to the param list
		int xlen  = strlen(info->name) + 1 + 2;
		char *str = malloc(xlen);

		snprintf(str, xlen, "lb%s", info->name);
		osl_strings_add(params, str);
		snprintf(str, xlen, "ub%s", info->name);
		osl_strings_add(params, str);

		free(str);
	}

	// gen next nested loop
	atiling_gen_iloop(output, fragment, loop_index + 1, params, ilevel + 1);

	// If we are generating the last loop
	if (loop_index + 1 == fragment->loop_count) {
		atiling_gen_iinner_loop(output, fragment, ilevel);
	}

	if (!(fragment->divs[loop_index][0] == 0 &&
		  fragment->divs[loop_index][1] == 0)) {
		atiling_gen_ifor_end(output, ilevel);
	}

	if (loop_index == 0) {
		osl_strings_free(params);
	}
}
