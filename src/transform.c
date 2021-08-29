#include <stdint.h>
#include <string.h>

#include "math_support.h"
#include "osl_pluto.h"
#include "pluto.h"
#include "pluto/pluto.h"
#include "post_transform.h"
#include "program.h"
#include "transforms.h"
#include "version.h"

#include "clan/clan.h"

#include "osl/extensions/irregular.h"
#include "osl/scop.h"

#include "atiling/fragment.h"
#include "atiling/macros.h"

void atiling_apply_transform(atiling_fragment_p frag) {
	ATILING_debug("Starting pluto optimization");

	PlutoContext *context			  = pluto_context_alloc();
	context->options->isldep		  = 1;
	context->options->tile			  = 1;
	context->options->parallel		  = 1;
	context->options->silent		  = 1;
	context->options->fuse			  = kNoFuse;
	context->options->debug			  = 0;
	context->options->diamondtile	  = 0;
	context->options->fulldiamondtile = 0;

	PlutoProg *prog = osl_scop_to_pluto_prog(frag->scop, context);

	// Auto transformation
	ATILING_debug("Pluto: auto_transform");
	pluto_auto_transform(prog);

	// Dependency calculation
	ATILING_debug("Pluto: dep_calculation");
	pluto_compute_dep_directions(prog);
	pluto_compute_dep_satisfaction(prog);

	ATILING_debug("Pluto: tiling");
	pluto_tile(prog);

	ATILING_debug("Pluto: intratileopt");
	pluto_intra_tile_optimize(prog, 0);

	// WIP
	osl_scop_p pluto_scop = osl_scop_clone(frag->scop);

	pluto_populate_scop(pluto_scop, prog, context);

	if (ATILING_DEBUG) {
		FILE *tmp = fopen("pluto.scop", "w");
		if (tmp == NULL) {
			ATILING_error("tmp fopen");
		}

		osl_scop_print(tmp, pluto_scop);
		fclose(tmp);
	}

	osl_statement_p pluto_stmt = pluto_scop->statement;
	osl_statement_p out_stmt   = frag->scop->statement;

	osl_scatnames_p out_scatnames =
		osl_generic_lookup(frag->scop->extension, OSL_URI_SCATNAMES);
	int scatnames_len			= osl_strings_size(out_scatnames->names);
	osl_strings_p current_order = NULL;

	while (out_stmt != NULL) {
		osl_relation_p out_scat	  = out_stmt->scattering;
		osl_relation_p pluto_scat = pluto_stmt->scattering;

		osl_body_p pluto_body =
			osl_generic_lookup(pluto_stmt->extension, OSL_URI_BODY);
		osl_body_p out_body =
			osl_generic_lookup(out_stmt->extension, OSL_URI_BODY);

		int it_len	= osl_strings_size(pluto_body->iterators);
		int tile_it = it_len / 2;

		// First we get best order for this statement according to pluto
		osl_strings_p order = osl_strings_malloc();
		// For each non tiled pluto iterator
		for (int i = 0; i < pluto_scat->nb_rows; i++) {
			for (int j = 0; j < it_len - tile_it; j++) {
				if (!osl_int_zero(
						out_scat->precision,
						pluto_scat->m[i][1 + pluto_scat->nb_output_dims +
										 tile_it + j])) {
					osl_strings_add(order,
									pluto_body->iterators->string[tile_it + j]);
				}
			}
		}
		ATILING_debug_call(osl_strings_print(stderr, order));
		int order_len = osl_strings_size(order);
		int it_idx	  = 0;

		// Then we update output scattering func
		for (int i = 0; i < scatnames_len && i < out_scat->nb_rows; i++) {
			int ok = ATILING_FALSE;
			// We check this scat dim is a iterator in the order
			for (int j = 0; j < order_len; j++) {
				if (!strcmp(out_scatnames->names->string[i],
							order->string[j])) {
					ok = ATILING_TRUE;
					break;
				}
			}
			// if not we go to next dim
			if (ok == ATILING_FALSE) {
				continue;
			}

			// if it is, this must be the first in the order
			for (int j = 0; j < out_scat->nb_input_dims; j++) {
				int val = strcmp(out_body->iterators->string[j],
								 order->string[it_idx]) == 0;
				osl_int_set_si(
					out_scat->precision,
					&out_scat->m[i][1 + out_scat->nb_output_dims + j], val);
			}

			it_idx++;
		}

		if (current_order != NULL) {
			if (osl_strings_size(current_order) < order_len) {
				osl_strings_free(current_order);
				current_order = order;
			} else {
				osl_strings_free(order);
			}
		} else {
			current_order = order;
		}

		out_stmt   = out_stmt->next;
		pluto_stmt = pluto_stmt->next;
	}
	fprintf(stderr, "[ATILING] Info: New order = ( ");
	int len = osl_strings_size(current_order);
	for (int i = 0; i < len; i++) {
		int j = 2 * i + 1;
		if (j >= scatnames_len)
			break;

		fprintf(stderr, "%s ", current_order->string[i]);

		if (strcmp(current_order->string[i], out_scatnames->names->string[j])) {
			free(out_scatnames->names->string[j]);
			ATILING_strdup(out_scatnames->names->string[j],
						   current_order->string[i]);
		}
	}
	printf(")\n");

	char *str = osl_scatnames_sprint(out_scatnames);
	ATILING_debug_call(fprintf(stderr, "%s\n", str));
	free(str);

	osl_scop_free(pluto_scop);
	pluto_context_free(context);
}