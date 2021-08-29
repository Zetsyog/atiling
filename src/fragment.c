#include "atiling/atiling.h"
#include "string.h"
#include <cloog/cloog.h>
#include <stdio.h>

atiling_fragment_p atiling_fragment_malloc() {
	atiling_fragment_p frag;
	ATILING_malloc(frag, sizeof(atiling_fragment_t));
	frag->path		 = NULL;
	frag->scop		 = NULL;
	frag->start[0]	 = 0;
	frag->start[1]	 = 0;
	frag->end[0]	 = 0;
	frag->end[1]	 = 0;
	frag->div_len	 = 0;
	frag->divs		 = NULL;
	frag->loop_count = 0;
	frag->loops		 = NULL;
	frag->id		 = 0;
	frag->options	 = NULL;
	return frag;
}

void atiling_fragment_free(atiling_fragment_p fragment) {
	for (int i = 0; i < fragment->loop_count; i++) {
		atiling_loop_info_free(fragment->loops[i]);
	}
	free(fragment->loops);
	for (int i = 0; fragment->divs[i] != NULL; i++) {
		free(fragment->divs[i]);
	}

	osl_scop_free(fragment->scop);

	free(fragment->path);
	free(fragment->divs);
	free(fragment);
}

static void get_loop_data(atiling_fragment_p frag, CloogOptions *options,
						  struct clast_for *loop, int depth) {
	char *buffer = NULL;
	size_t len, str_len;

	FILE *buf_fd = open_memstream(&buffer, &len);
	if (buf_fd == NULL) {
		ATILING_error("fmemopen");
	}

	clast_pprint_expr(options, buf_fd, loop->LB);
	fprintf(buf_fd, "%c", 0);
	fflush(buf_fd);

	str_len = strlen(buffer);

	ATILING_malloc(frag->loops[depth]->lb, str_len + 1);
	snprintf(frag->loops[depth]->lb, str_len + 1, "%s", buffer);

	fclose(buf_fd);
	free(buffer);

	buffer = NULL;
	buf_fd = open_memstream(&buffer, &len);
	if (buf_fd == NULL) {
		ATILING_error("fmemopen");
	}

	clast_pprint_expr(options, buf_fd, loop->UB);
	fprintf(buf_fd, "%c", 0);
	fflush(buf_fd);

	str_len = strlen(buffer);

	ATILING_malloc(frag->loops[depth]->ub, str_len + 1);
	snprintf(frag->loops[depth]->ub, str_len + 1, "%s", buffer);

	fclose(buf_fd);
	free(buffer);
}

static void browse_loops_rec(atiling_fragment_p frag, CloogOptions *options,
							 struct clast_stmt *f, int depth) {
	struct clast_stmt *it = f;

	for (; it; it = it->next) {
		if (CLAST_STMT_IS_A(it, stmt_root)) {
			continue;
		} else if (CLAST_STMT_IS_A(it, stmt_for)) {
			struct clast_for *loop = (struct clast_for *)it;
			get_loop_data(frag, options, loop, depth);

			browse_loops_rec(frag, options, loop->body, depth + 1);
		} else if (CLAST_STMT_IS_A(it, stmt_block)) {
			// if its a block we want to see if there is any for loop inside but
			// loop depth is still the same
			browse_loops_rec(frag, options, ((struct clast_block *)it)->body,
							 depth);
		} else if (CLAST_STMT_IS_A(it, stmt_guard)) {
			// same for guard, we need to look deeper
			browse_loops_rec(frag, options, ((struct clast_guard *)it)->then,
							 depth);
		}
	}
}

void atiling_fragment_get_loops_bound(atiling_fragment_p frag) {
	osl_scop_p scop = osl_scop_clone(frag->scop);
	// Get cloog infos
	CloogInput *input;
	CloogOptions *cloogOptions;
	CloogState *state;
	state		 = cloog_state_malloc();
	cloogOptions = cloog_options_malloc(state);
	struct clast_stmt *root;

	cloogOptions->f		= -1;
	cloogOptions->sh	= 1;
	cloogOptions->otl	= 1;
	cloogOptions->quiet = 1;

	osl_scatnames_p scatnames =
		osl_generic_lookup(scop->extension, OSL_URI_SCATNAMES);
	int scatnames_len = osl_strings_size(scatnames->names);
	for (int i = 1; i < scatnames_len; i += 2) {
		char *old = scatnames->names->string[i];
		// new string will be ub[old] so new len is old + 2 + 1
		// one more for the terminating 0 char
		scatnames->names->string[i] = malloc(strlen(old) + 2 + 1);
		sprintf(scatnames->names->string[i], "%s%s", "ub", old);
		free(old);
	}

	input = cloog_input_from_osl_scop(state, scop);
	cloog_options_copy_from_osl_scop(scop, cloogOptions);
	root = cloog_clast_create_from_input(input, cloogOptions);

	// Do the magic here
	browse_loops_rec(frag, cloogOptions, root, 0);

	cloog_clast_free(root);

	cloog_options_free(cloogOptions);
	cloog_state_free(state);
}

void atiling_fragment_idump(FILE *file, atiling_fragment_p frag, int level) {
	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}

	if (frag != NULL) {
		fprintf(file, "+-- atiling_fragment_t \n");
	} else {
		fprintf(file, "+-- NULL frag \n");
	}

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   start: %i,%i\n", frag->start[0], frag->start[1]);

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   end: %i,%i\n", frag->end[0], frag->end[1]);

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   divs:\n");

	for (int i = 0; i < frag->div_len; i++) {
		// indent
		for (int i = 0; i < level; i++) {
			fprintf(file, "|\t");
		}
		fprintf(file, "|\t|   %s\n", frag->divs[i]);
	}

	// indent
	for (int i = 0; i < level; i++) {
		fprintf(file, "|\t");
	}
	fprintf(file, "|   loops:\n");

	for (int i = 0; i < frag->loop_count; i++) {
		// indent
		for (int i = 0; i < level + 1; i++) {
			fprintf(file, "|\t");
		}
		fprintf(file, "%s\n", frag->loops[i]->it);
		// indent
		for (int i = 0; i < level + 2; i++) {
			fprintf(file, "|\t");
		}
		fprintf(file, "lb: %s\n", frag->loops[i]->lb);
		// indent
		for (int i = 0; i < level + 2; i++) {
			fprintf(file, "|\t");
		}
		fprintf(file, "ub: %s\n", frag->loops[i]->ub);
	}
}

/**
 * @param[in] fragment
 * @param[in] depth
 * @return whether the loop of given depth must be tiled or not
 */
int is_tiling_enabled(atiling_fragment_p fragment, int depth) {
	if (depth < fragment->loop_count) {
		if (fragment->divs[depth][0] == '1' && fragment->divs[depth][1] == 0) {
			return ATILING_FALSE;
		}
	}
	return ATILING_TRUE;
}
