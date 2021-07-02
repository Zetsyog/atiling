#include "atiling/atiling.h"
#include <clan/clan.h>
#include <osl/osl.h>
#include <stdio.h>
#include <string.h>

void atiling_gen_fragment(FILE *, atiling_fragment_p);
void atiling_gen_trahrhe(atiling_fragment_p);
void atiling_gen_iloop(FILE *, atiling_fragment_p, int, osl_strings_p, int);
void gen_macros(FILE *);

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
		ATILING_error("null input");

	rewind(input);

	gen_macros(output);

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
	if (fragment->loop_count == 0) {
		return;
	}

	fprintf(output, "\n");
	fprintf(output, "// begin atiling\n");

	osl_strings_p params =
		osl_strings_clone(fragment->loops[0]->parameters_names);
	atiling_gen_iloop(output, fragment, 0, params, 0);

	fprintf(output, "// end atiling\n");

	atiling_gen_trahrhe(fragment);

	osl_strings_free(params);
}

void atiling_gen_indent(FILE *file, int level) {
	for (int i = 0; i < level; i++) {
		fprintf(file, "\t");
	}
}

void atiling_gen_ixpcmax(FILE *output, char *it_name, char **params,
						 int level) {
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

void atiling_gen_ivol_level(FILE *output, int level, char *it_name, char *div,
							int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "%s%i = %s%s / %s;\n", ATILING_GEN_STR_TILEVOL, level,
			it_name, ATILING_GEN_STR_PCMAX, div);
}

void atiling_gen_iubxt(FILE *output, char *x, int vol_level, int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "ub%st = max(%s%s / (%s%i) - 1, 0);\n", x, x,
			ATILING_GEN_STR_PCMAX, ATILING_GEN_STR_TILEVOL, vol_level);
}

void atiling_gen_iomp_pragma(FILE *output, atiling_fragment_p frag, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "#pragma omp parallel for ");
	fprintf(output, "firstprivate(%s%s, %s%i) \\\n", frag->loops[0]->name,
			ATILING_GEN_STR_PCMAX, ATILING_GEN_STR_TILEVOL, 1);
	atiling_gen_indent(output, level + 3);
	fprintf(output, "private(");
	for (int i = 0; i < frag->loop_count; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", frag->loops[i]->name);
		fprintf(output, ", %st", frag->loops[i]->name);
		fprintf(output, ", lb%s", frag->loops[i]->name);
		fprintf(output, ", ub%s", frag->loops[i]->name);
		if (i != 0) {
			fprintf(output, ", %s%s", frag->loops[i]->name,
					ATILING_GEN_STR_PCMAX);
			fprintf(output, ", %s%i", ATILING_GEN_STR_TILEVOL, i + 1);
		}
	}
	fprintf(output, ")\n");
}

void atiling_gen_ifor_begin(FILE *output, char *x, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "for(%st = 0; %st <= ub%st; %st++) {\n", x, x, x, x);
}

void atiling_gen_ifor_end(FILE *output, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "}\n");
}

void atiling_gen_ilbx(FILE *output, char *x, char **params, int level,
					  int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "lb%s = %s%s%s(max(%st * (%s%i + 1), 1), ", x, x,
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
	fprintf(output, "ub%s = %s%s%s(min((%st + 1) * (%s%i + 1), %s%s), ", x, x,
			ATILING_GEN_STR_TRAHRHE, x, x, ATILING_GEN_STR_TILEVOL, level, x,
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
		char *x = fragment->loops[i]->name;

		atiling_gen_indent(output, ilevel + 1 + i);

		if (fragment->divs[i][0] == '1' && fragment->divs[i][1] == 0) {
			fprintf(output, "for(%s = ", x);
			loop_info_bound_print(output, fragment->loops[i],
								  fragment->loops[i]->start_row, NULL);
			fprintf(output, " ; %s <= ", x);
			loop_info_bound_print(output, fragment->loops[i],
								  fragment->loops[i]->end_row, NULL);
			fprintf(output, " ; %s++) {\n", x);
		} else {
			fprintf(output, "for(%s = max(", x);
			loop_info_bound_print(output, fragment->loops[i],
								  fragment->loops[i]->start_row, NULL);
			fprintf(output, ",lb%s); %s <= min(", x, x);
			loop_info_bound_print(output, fragment->loops[i],
								  fragment->loops[i]->end_row, NULL);
			fprintf(output, ", ub%s); %s++) {\n", x, x);
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

void atiling_gen_islice_adjust(FILE *output, char *x, loop_info_p info,
							   int ilevel) {
	atiling_gen_indent(output, ilevel);
	// Last slice adjustment
	fprintf(output, "if(%st == ub%st) ub%s = ", x, x, x);
	loop_info_bound_print(output, info, info->end_row, "ub");
	fprintf(output, ";\n");
}

void atiling_gen_ivardecl(FILE *output, loop_info_p info, int depth,
						  int ilevel) {
	atiling_gen_indent(output, ilevel);

	// iterator
	fprintf(output, "%s %s = 0, ", ATILING_GEN_VAR_TYPE, info->name);

	// xt
	fprintf(output, "%st = 0, ", info->name);

	// lbx ubx
	fprintf(output, "lb%s = 0, ub%s = 0, ", info->name, info->name);

	// x_pcmax
	fprintf(output, "%s%s = 0, ", info->name, ATILING_GEN_STR_PCMAX);

	// TILE_VOL_Li
	fprintf(output, "%s%i = 0, ", ATILING_GEN_STR_TILEVOL, depth + 1);

	// ubxt
	fprintf(output, "ub%st = 0; \n", info->name);
}

void atiling_gen_iloop(FILE *output, atiling_fragment_p fragment,
					   int loop_index, osl_strings_p params, int ilevel) {
	// stop condition
	if (loop_index >= fragment->loop_count)
		return;

	osl_scop_p scop	 = fragment->scop;
	loop_info_p info = fragment->loops[loop_index];

	if (loop_index == 0) {
		for (int i = 0; i < fragment->loop_count; i++) {
			atiling_gen_ivardecl(output, fragment->loops[i], i, ilevel);
		}
		fprintf(output, "\n");
	}

	// div == "1" means that no transformation is done on this loop
	if (!(fragment->divs[loop_index][0] == '1' &&
		  fragment->divs[loop_index][1] == 0)) {

		atiling_gen_ixpcmax(output, info->name, params->string, ilevel);
		atiling_gen_ivol_level(output, loop_index + 1, info->name,
							   fragment->divs[loop_index], ilevel);
		atiling_gen_iubxt(output, info->name, loop_index + 1, ilevel);
		fprintf(output, "\n");

		if (loop_index == 0) {
			atiling_gen_iomp_pragma(output, fragment, ilevel);
		}
		atiling_gen_ifor_begin(output, info->name, ilevel);

		atiling_gen_ilbx(output, info->name, params->string, loop_index + 1,
						 ilevel + 1);
		atiling_gen_iubx(output, info->name, params->string, loop_index + 1,
						 ilevel + 1);
		atiling_gen_islice_adjust(output, info->name, info, ilevel + 1);

		fprintf(output, "\n");

		// Append lbx and ubx to the param list
		int xlen  = strlen(info->name) + 1 + 2;
		char *str = malloc(xlen);

		snprintf(str, xlen, "lb%s", info->name);
		osl_strings_add(params, str);
		snprintf(str, xlen, "ub%s", info->name);
		osl_strings_add(params, str);

		free(str);

		// gen next nested loop
		atiling_gen_iloop(output, fragment, loop_index + 1, params, ilevel + 1);
	} else {
		// gen next nested loop
		atiling_gen_iloop(output, fragment, loop_index + 1, params, ilevel);
	}

	// If we are generating the last loop
	if (loop_index + 1 == fragment->loop_count) {
		atiling_gen_iinner_loop(output, fragment, ilevel);
	}

	if (!(fragment->divs[loop_index][0] == '1' &&
		  fragment->divs[loop_index][1] == 0)) {
		atiling_gen_ifor_end(output, ilevel);
	}
}

#define MAX_STR 512

void atiling_gen_trahrhe(atiling_fragment_p fragment) {
	char cmd[MAX_STR];
	int cursor = 0;

	cursor += sprintf(cmd + cursor, "[");
	osl_scop_p scop = fragment->scop;

	int param_size = osl_strings_size(fragment->loops[0]->parameters_names);
	for (int i = 0; i < param_size; i++) {
		if (i != 0) {
			cursor += sprintf(cmd + cursor, ", ");
		}
		cursor += sprintf(cmd + cursor, "%s",
						  fragment->loops[0]->parameters_names->string[i]);
	}

	cursor += sprintf(cmd + cursor, "]");
	cursor += sprintf(cmd + cursor, " -> { [");

	for (int i = 0; i < fragment->loop_count; i++) {
		if (i != 0) {
			cursor += sprintf(cmd + cursor, ", ");
		}
		cursor += sprintf(cmd + cursor, "%s", fragment->loops[i]->name);
	}

	cursor += sprintf(cmd + cursor, "] : ");

	loop_info_p inner_loop = fragment->loops[fragment->loop_count - 1];

	for (int i = 0; i < inner_loop->relation->nb_rows; i++) {
		if (i != 0) {
			cursor += sprintf(cmd + cursor, " and ");
		}
		char *expr = osl_relation_expression(inner_loop->relation, i,
											 inner_loop->name_array);

		cursor += sprintf(cmd + cursor, "%s >= 0", expr);

		free(expr);
	}

	cursor += sprintf(cmd + cursor, " }");
	printf("%s\n", cmd);
}

void gen_macros(FILE *output) {
	fprintf(output, "#include \"%s\"\n", ATILING_GEN_INCLUDE);
	fprintf(output, "%s\n", ATILING_GEN_MIN);
	fprintf(output, "%s\n", ATILING_GEN_MAX);
}
