#include "atiling/atiling.h"
#include "cloog/cloog.h"
#include "math_support.h"
#include "osl_pluto.h"
#include "pluto.h"
#include "pluto/pluto.h"
#include "post_transform.h"
#include "program.h"
#include "transforms.h"
#include "version.h"
#include <libgen.h>
#include <osl/osl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static void atiling_gen_fragment(FILE *, atiling_fragment_p, atiling_options_p);
static void atiling_sprint_trahrhe(char *s, atiling_fragment_p);
static void atiling_gen_iloop(FILE *, atiling_fragment_p, int, osl_strings_p,
							  int);
static void gen_macros(FILE *, char *, int);
static void atiling_gen_info_file(atiling_fragment_p, atiling_options_p);

/**
 * @brief Generate new code with algebraic tiling and write it in output
 * This function will copy the code of input file in output, except the parts
 * surrounded by the pragmas that will be replaced by the tiled loops
 * @param input			The input file
 * @param output		The output file
 * @param fragment_list	The list of fragments
 * @param options		The program options
 */
void atiling_gen(FILE *input, FILE *output, atiling_fragment_p fragment,
				 atiling_options_p options) {
	int line   = 1;
	int column = 1;
	int ignore = ATILING_FALSE;
	int c;

	if (input == NULL) {
		ATILING_error("null input");
	}

	rewind(input);

	gen_macros(output, options->output, fragment->id);
	while ((c = fgetc(input)) != EOF) {
		if (c == '\n') {
			line++;
			column = 1;
		}

		if (fragment != NULL) {
			if (line == fragment->start[0] && column == fragment->start[1]) {
				ATILING_debug("starting gen");
				ignore = ATILING_TRUE;
				atiling_gen_fragment(output, fragment, options);
			}
		}

		if (ignore == ATILING_FALSE) {
			fprintf(output, "%c", (char)c);
		}

		if (fragment != NULL) {
			if (ignore == ATILING_TRUE && line == fragment->end[0] &&
				column == fragment->end[1]) {
				ignore = ATILING_FALSE;
				ATILING_debug("ending gen");
			}
		}

		column++;
	}
}

/**
 * @brief Generates the new code for a given fragment
 * @param output	The output file
 * @param fragment 	The fragment
 */
void atiling_gen_fragment(FILE *output, atiling_fragment_p fragment,
						  atiling_options_p options) {
	if (fragment->loop_count == 0) {
		return;
	}

	fprintf(output, "\n");
	fprintf(output, "// begin atiling\n");

	osl_strings_p params =
		osl_strings_clone(fragment->loops[0]->parameters_names);
	atiling_gen_iloop(output, fragment, 0, params, 0);

	fprintf(output, "// end atiling\n");

	atiling_gen_info_file(fragment, options);

	osl_strings_free(params);
}

/**
 * @brief Write tabs to indent file to the given level
 * @param file 	The output file
 * @param level	The indent level
 */
void atiling_gen_indent(FILE *file, int level) {
	for (int i = 0; i < level; i++) {
		fprintf(file, "\t");
	}
}

static void atiling_gen_ixpcmax(FILE *output, char *it_name, char **params,
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

static void atiling_gen_ivol_level(FILE *output, int id, int level,
								   char *it_name, char *div, int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "%s%i_%i = %s%s / %s;\n", ATILING_GEN_STR_TILEVOL, id,
			level, it_name, ATILING_GEN_STR_PCMAX, div);
}

static void atiling_gen_iubxt(FILE *output, char *x, int vol_level, int id,
							  int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "ub%st = max(%s%s / (%s%i_%i) - 1, 0);\n", x, x,
			ATILING_GEN_STR_PCMAX, ATILING_GEN_STR_TILEVOL, vol_level, id);
}

static void atiling_gen_iomp_pragma(FILE *output, atiling_fragment_p frag,
									int level) {
	int first_depth;
	for (first_depth = 0; first_depth < frag->loop_count; first_depth++) {
		if (is_tiling_enabled(frag, first_depth)) {
			break;
		}
	}

	if (first_depth >= frag->loop_count) {
		return;
	}

	atiling_gen_indent(output, level);
	fprintf(output, "#pragma omp parallel for ");
	fprintf(output, "firstprivate(%s%s, %s%i_%i, ub%st) \\\n",
			frag->loops[first_depth]->name, ATILING_GEN_STR_PCMAX,
			ATILING_GEN_STR_TILEVOL, 1, frag->id,
			frag->loops[first_depth]->name);
	atiling_gen_indent(output, level + 3);
	fprintf(output, "private(");

	for (int i = first_depth; i < frag->loop_count; i++) {

		if (i != first_depth) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", frag->loops[i]->name);
		if (is_tiling_enabled(frag, i)) {
			fprintf(output, ", %st", frag->loops[i]->name);
			fprintf(output, ", lb%s", frag->loops[i]->name);
			fprintf(output, ", ub%s", frag->loops[i]->name);
			if (i != first_depth) {
				fprintf(output, ", %s%s", frag->loops[i]->name,
						ATILING_GEN_STR_PCMAX);
				fprintf(output, ", %s%i_%i", ATILING_GEN_STR_TILEVOL, i + 1,
						frag->id);
				fprintf(output, ", ub%st", frag->loops[i]->name);
			}
		}
	}
	fprintf(output, ")\n");
}

static void atiling_gen_ifor_begin(FILE *output, char *x, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "for(%st = 0; %st <= ub%st; %st++) {\n", x, x, x, x);
}

static void atiling_gen_ifor_end(FILE *output, int level) {
	atiling_gen_indent(output, level);
	fprintf(output, "}\n");
}

static void atiling_gen_ilbx(FILE *output, char *x, char **params, int level,
							 int id, int ilevel) {
	atiling_gen_indent(output, ilevel);
	fprintf(output, "lb%s = %s%s%s(max(%st * (%s%i_%i), 1), ", x, x,
			ATILING_GEN_STR_TRAHRHE, x, x, ATILING_GEN_STR_TILEVOL, level, id);

	for (int i = 0; params[i] != NULL; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", params[i]);
	}
	fprintf(output, ");\n");
}

static void atiling_gen_iubx(FILE *output, char *x, char **params, int level,
							 int id, int ilevel) {
	atiling_gen_indent(output, ilevel);

	// trahrhe i(min(( it+1)∗(TILE VOL L1+1),i pcmax),N, M) − 1;
	fprintf(output, "ub%s = %s%s%s(min((%st + 1) * (%s%i_%i), %s%s), ", x, x,
			ATILING_GEN_STR_TRAHRHE, x, x, ATILING_GEN_STR_TILEVOL, level, id,
			x, ATILING_GEN_STR_PCMAX);

	for (int i = 0; params[i] != NULL; i++) {
		if (i != 0) {
			fprintf(output, ", ");
		}
		fprintf(output, "%s", params[i]);
	}
	fprintf(output, ") - 1;\n");
}

static void atiling_gen_cloog_code(FILE *output, osl_scop_p scop, int level) {
	CloogInput *input;
	CloogOptions *cloogOptions;
	CloogState *state;
	state		 = cloog_state_malloc();
	cloogOptions = cloog_options_malloc(state);
	struct clast_stmt *root;
	int nstmts		  = 1;
	cloogOptions->sh  = 1;
	cloogOptions->otl = 1;

	cloogOptions->quiet = 1;

	input = cloog_input_from_osl_scop(state, scop);
	cloog_options_copy_from_osl_scop(scop, cloogOptions);

	root = cloog_clast_create_from_input(input, cloogOptions);

	clast_pprint(output, root, 0, cloogOptions);

	cloog_clast_free(root);

	cloog_options_free(cloogOptions);
	cloog_state_free(state);
}

static void atiling_gen_domain(osl_statement_p stmt, int orig_params, int it) {
	osl_relation_p base = stmt->domain;
	int precision		= base->precision;

	osl_vector_p vec;

	while (base != NULL) {

		vec = osl_vector_pmalloc(precision, base->nb_columns);

		// i - lbi >= 0
		osl_int_set_si(precision, &(vec->v[0]), 1);		 // >= 0
		osl_int_set_si(precision, &(vec->v[1 + it]), 1); // i
		osl_int_set_si(
			precision,
			&(vec->v[1 + base->nb_output_dims + orig_params + it * 2]),
			-1); // -lbi

		osl_relation_insert_vector(base, vec, -1);
		osl_vector_free(vec);

		vec = osl_vector_pmalloc(precision, base->nb_columns);

		// -i + ubi >= 0
		osl_int_set_si(precision, &(vec->v[0]), 1);		  // >= 0
		osl_int_set_si(precision, &(vec->v[1 + it]), -1); // -i
		osl_int_set_si(
			precision,
			&(vec->v[1 + base->nb_output_dims + orig_params + it * 2 + 1]),
			1); // ubi

		osl_relation_insert_vector(base, vec, -1);
		osl_vector_free(vec);

		vec = osl_vector_pmalloc(precision, base->nb_columns);

		// ubi >= 0 && ubi -lbi >= 0
		osl_int_set_si(precision, &(vec->v[0]), 1); // >= 0
		osl_int_set_si(
			precision,
			&(vec->v[1 + base->nb_output_dims + orig_params + it * 2 + 1]),
			1); // ubi

		osl_relation_insert_vector(base, vec, -1);

		osl_int_set_si(
			precision,
			&(vec->v[1 + base->nb_output_dims + orig_params + it * 2]),
			-1); // -lbi
		osl_relation_insert_vector(base, vec, -1);

		osl_vector_free(vec);

		vec = osl_vector_pmalloc(precision, base->nb_columns);

		// -i + ubi >= 0
		osl_int_set_si(precision, &(vec->v[0]), 1);		  // >= 0
		osl_int_set_si(precision, &(vec->v[1 + it]), -1); // -i
		osl_int_set_si(
			precision,
			&(vec->v[1 + base->nb_output_dims + orig_params + it * 2 + 1]),
			1); // ubi

		osl_relation_insert_vector(base, vec, -1);
		osl_vector_free(vec);

		base = base->next;
	}
}

static void atiling_gen_update_domain(atiling_fragment_p frag,
									  osl_statement_p stmt, int orig_params) {
	int precision = stmt->domain->precision;
	int num_it	  = 0;
	osl_body_p body =
		(osl_body_p)osl_generic_lookup(stmt->extension, OSL_URI_BODY);
	if (body) {
		num_it = osl_strings_size(body->iterators);
	}

	for (int i = 0; i < num_it; i++) {
		if (is_tiling_enabled(frag, i)) {
			atiling_gen_domain(stmt, orig_params, i);
		}
	}
}

/**
 * Generates the inner loops
 */
static void atiling_gen_iinner_loop(FILE *output, atiling_fragment_p frag,
									int ilevel) {
	osl_scop_p scop	 = osl_scop_clone(frag->scop);
	int orig_params	 = frag->scop->context->nb_parameters;
	int added_params = frag->loop_count * 2;
	int precision	 = scop->statement->domain->precision;

	// Update context info
	// We add two params (and row cols) for each iterator x : lbx and ubx
	scop->context->nb_parameters += added_params;
	scop->context->nb_columns += added_params;

	// Add param names
	osl_strings_p params = scop->parameters->data;
	for (int i = 0; i < frag->loop_count; i++) {
		char *str = malloc(strlen(frag->loops[i]->name) + 2 + 1);
		sprintf(str, "%s%s", "lb", frag->loops[i]->name);
		osl_strings_add(params, str);
		sprintf(str, "%s%s", "ub", frag->loops[i]->name);
		osl_strings_add(params, str);
		free(str);
	}

	// For each statement
	osl_statement_p stmt = scop->statement;
	while (stmt != NULL) {
		// Insert new cols
		for (int i = 0; i < added_params; i++) {
			osl_relation_p domain = stmt->domain;

			while (domain != NULL) {
				osl_relation_insert_blank_column(domain,
												 domain->nb_columns - 1);
				domain->nb_parameters++;

				domain = domain->next;
			}
			osl_relation_insert_blank_column(stmt->scattering,
											 stmt->scattering->nb_columns - 1);

			osl_relation_list_p list = stmt->access;
			while (list != NULL) {
				osl_relation_insert_blank_column(list->elt,
												 list->elt->nb_columns - 1);
				list = list->next;
			}
		}

		stmt->scattering->nb_parameters += added_params;

		osl_relation_list_p list = stmt->access;
		while (list != NULL) {
			list->elt->nb_parameters += added_params;
			list = list->next;
		}

		// Update domain
		atiling_gen_update_domain(frag, stmt, orig_params);

		stmt = stmt->next;
	}

	FILE *tmp = fopen("inner.scop", "w");
	osl_scop_print(tmp, scop);
	fclose(tmp);

	atiling_gen_cloog_code(output, scop, ilevel);

	// osl_scop_free(scop);
}

static void atiling_gen_islice_adjust(FILE *output, char *x, loop_info_p info,
									  int ilevel) {
	atiling_gen_indent(output, ilevel);
	// Last slice adjustment
	fprintf(output, "if(%st == ub%st) ub%s = ", x, x, x);
	atiling_loop_info_bound_print(output, info, info->end_row, "ub");
	fprintf(output, ";\n");
}

static void atiling_gen_ivolume_guard(FILE *output, char *name, int level,
									  int id, int ilevel) {
	//	if (ubi < lbi) { fprintf(stderr, "\nThe tile volume of level 1 (%ld)
	// seems too small, raise it and try again\n",TILE_VOL_L1_0); exit(1); }
	atiling_gen_indent(output, ilevel);
	fprintf(output, "if (ub%s < lb%s ) {\n", name, name);
	atiling_gen_indent(output, ilevel + 1);
	fprintf(output,
			"fprintf(stderr, \"The tile volume of level %i (%%ld) "
			"seems too small, raise it and try again\\n\", %s%i_%i);\n",
			level, ATILING_GEN_STR_TILEVOL, level, id);
	atiling_gen_indent(output, ilevel + 1);
	fprintf(output, "exit(1);\n");
	atiling_gen_indent(output, ilevel);
	fprintf(output, "}\n");
}

static void atiling_gen_ivardecl(FILE *output, loop_info_p info, int depth,
								 int id, int ilevel) {
	atiling_gen_indent(output, ilevel);

	// iterator
	fprintf(output, "%s ", ATILING_GEN_VAR_TYPE);

	// xt
	fprintf(output, "%st = 0, ", info->name);

	// lbx ubx
	fprintf(output, "lb%s = 0, ub%s = 0, ", info->name, info->name);

	// x_pcmax
	fprintf(output, "%s%s = 0, ", info->name, ATILING_GEN_STR_PCMAX);

	// TILE_VOL_Li
	fprintf(output, "%s%i_%i = 0, ", ATILING_GEN_STR_TILEVOL, depth + 1, id);

	// ubxt
	fprintf(output, "ub%st = 0; \n", info->name);
}

/**
 * Generate the code for the loop of depth loop_index with base indentation
 * level ilevel This is a recursive function on loop_index
 * @param output		The output file
 * @param fragment		The current fragment
 * @param loop_index	The loop index (loop depth: 0 -> external loop,...)
 * @param params		The parameters. Used for trahrhe computation functions
 * @param ilevel		The indent level
 */
static void atiling_gen_iloop(FILE *output, atiling_fragment_p fragment,
							  int loop_index, osl_strings_p params,
							  int ilevel) {
	// stop condition
	if (loop_index >= fragment->loop_count)
		return;

	osl_scop_p scop	 = fragment->scop;
	loop_info_p info = fragment->loops[loop_index];

	if (loop_index == 0) {
		for (int i = 0; i < fragment->loop_count; i++) {
			if (is_tiling_enabled(fragment, i)) {
				atiling_gen_ivardecl(output, fragment->loops[i], i,
									 fragment->id, ilevel);
			}
		}
		fprintf(output, "\n");
	}

	// div == "1" means that no transformation is done on this loop
	if (is_tiling_enabled(fragment, loop_index)) {

		atiling_gen_ixpcmax(output, info->name, params->string, ilevel);
		atiling_gen_ivol_level(output, loop_index + 1, fragment->id, info->name,
							   fragment->divs[loop_index], ilevel);
		atiling_gen_iubxt(output, info->name, loop_index + 1, fragment->id,
						  ilevel);
		fprintf(output, "\n");

		if (loop_index == 0) {
			atiling_gen_iomp_pragma(output, fragment, ilevel);
		}
		atiling_gen_ifor_begin(output, info->name, ilevel);

		atiling_gen_ilbx(output, info->name, params->string, loop_index + 1,
						 fragment->id, ilevel + 1);
		atiling_gen_iubx(output, info->name, params->string, loop_index + 1,
						 fragment->id, ilevel + 1);
		atiling_gen_islice_adjust(output, info->name, info, ilevel + 1);

		atiling_gen_ivolume_guard(output, info->name, loop_index + 1,
								  fragment->id, ilevel + 1);

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

/**
 * Write the trahrhe domain argument inside a string
 */
void atiling_sprint_trahrhe(char *s, atiling_fragment_p fragment) {
	int cursor		= 0;
	osl_scop_p scop = fragment->scop;

	cursor += sprintf(s + cursor, "[");

	int param_size = 0;
	int loop_idx   = 0;
	for (int i = 0; i < fragment->loop_count; i++) {
		int k = osl_strings_size(fragment->loops[0]->parameters_names);
		if (k > param_size) {
			param_size = k;
			loop_idx   = i;
		}
	}

	for (int i = 0; i < param_size; i++) {
		if (i != 0) {
			cursor += sprintf(s + cursor, ", ");
		}
		cursor +=
			sprintf(s + cursor, "%s",
					fragment->loops[loop_idx]->parameters_names->string[i]);
	}

	cursor += sprintf(s + cursor, "]");
	cursor += sprintf(s + cursor, " -> { [");

	int written = ATILING_FALSE;
	for (int i = 0; i < fragment->loop_count; i++) {
		if (written) {
			cursor += sprintf(s + cursor, ", ");
		}
		cursor += sprintf(s + cursor, "%s", fragment->loops[i]->name);
		written = ATILING_TRUE;
	}

	cursor += sprintf(s + cursor, "] : ");

	loop_info_p inner_loop = fragment->loops[fragment->loop_count - 1];

	int first_expr = ATILING_TRUE;
	for (int i = 0; i < inner_loop->domain->nb_rows; i++) {

		if (!first_expr) {
			cursor += sprintf(s + cursor, " and ");
		}

		char *expr = osl_relation_expression(inner_loop->domain, i,
											 inner_loop->name_array);
		cursor += sprintf(s + cursor, "%s >= 0", expr);

		free(expr);

		first_expr = ATILING_FALSE;
	}

	cursor += sprintf(s + cursor, " }");
}

/**
 * Generate macros used in generated code
 * @param output The output file
 */
void gen_macros(FILE *output, char *out, int id) {
	fprintf(output, "#include <omp.h>\n");
	fprintf(output, "#include \"%s%i%s\"\n", basename(out), id,
			ATILING_GEN_INCLUDE);
	fprintf(output, "%s\n", ATILING_GEN_MIN);
	fprintf(output, "%s\n", ATILING_GEN_MAX);
}

/**
 * Generate .trahrhe file
 * This file is used by the trahrhe bash script
 * It contains on the first line the trahrhe domain arg
 * On the second line the name of the header used in #include
 * On the third line there is the number of tiled level
 */
void atiling_gen_info_file(atiling_fragment_p fragment,
						   atiling_options_p options) {
	pid_t pid;
	int ret;
	ATILING_debug("Generating trahrhe header");
	char cmd[256]	 = {0};
	int tiling_level = 0;

	atiling_sprint_trahrhe(cmd, fragment);

	for (int i = 0; i < fragment->loop_count; i++) {
		if (is_tiling_enabled(fragment, i)) {
			tiling_level++;
		}
	}

	// write .atrahrhe file to give bash script info
	FILE *trahrhe_tmp = fopen(".atrahrhe", "w");
	// first line is trahrhe domain
	fprintf(trahrhe_tmp, "%s\n", cmd);
	// second line is header name
	fprintf(trahrhe_tmp, "%s%i%s\n", basename(options->output), fragment->id,
			ATILING_GEN_INCLUDE);
	// third line is tile level
	fprintf(trahrhe_tmp, "%i\n", tiling_level);

	fclose(trahrhe_tmp);

	ATILING_debug_call(
		fprintf(stderr, "[ATILING] Debug: Trahrhe domain = %s\n", cmd));
	ATILING_debug_call(fprintf(
		stderr, "[ATILING] Debug: trahrhe tiling level = %i\n", tiling_level));

	ATILING_debug("Done.");
}
