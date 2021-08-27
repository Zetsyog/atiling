
#include "atiling/atiling.h"
#include <string.h>

/**
 * osl_relation_strings function:
 * this function creates a NULL-terminated array of strings from an
 * osl_names_t structure in such a way that the ith string is the "name"
 * corresponding to the ith column of the constraint matrix.
 * \param[in] relation The relation for which we need an array of names.
 * \param[in] names    The set of names for each element.
 * \return An array of strings with one string per constraint matrix column.
 */
char **osl_relation_strings(const osl_relation_t *relation,
							const osl_names_t *names) {
	char **strings;
	char temp[OSL_MAX_STRING];
	int i, offset;

	if ((relation == NULL) || (names == NULL)) {
		OSL_debug("no names or relation to build the name array");
		return NULL;
	}

	OSL_malloc(strings, char **,
			   ((size_t)relation->nb_columns + 1) * sizeof(char *));
	strings[relation->nb_columns] = NULL;

	// 1. Equality/inequality marker.
	OSL_strdup(strings[0], "e/i");
	offset = 1;

	// 2. Output dimensions.
	if (osl_relation_is_access(relation)) {
		// The first output dimension is the array name.
		OSL_strdup(strings[offset], "Arr");
		// The other ones are the array dimensions [1]...[n]
		for (i = offset + 1; i < relation->nb_output_dims + offset; i++) {
			sprintf(temp, "[%d]", i - 1);
			OSL_strdup(strings[i], temp);
		}
	} else if ((relation->type == OSL_TYPE_DOMAIN) ||
			   (relation->type == OSL_TYPE_CONTEXT)) {
		for (i = offset; i < relation->nb_output_dims + offset; i++) {
			OSL_strdup(strings[i], names->iterators->string[i - offset]);
		}
	} else {
		for (i = offset; i < relation->nb_output_dims + offset; i++) {
			OSL_strdup(strings[i], names->scatt_dims->string[i - offset]);
		}
	}
	offset += relation->nb_output_dims;

	// 3. Input dimensions.
	for (i = offset; i < relation->nb_input_dims + offset; i++)
		OSL_strdup(strings[i], names->iterators->string[i - offset]);
	offset += relation->nb_input_dims;

	// 4. Local dimensions.
	for (i = offset; i < relation->nb_local_dims + offset; i++)
		OSL_strdup(strings[i], names->local_dims->string[i - offset]);
	offset += relation->nb_local_dims;

	// 5. Parameters.
	for (i = offset; i < relation->nb_parameters + offset; i++)
		OSL_strdup(strings[i], names->parameters->string[i - offset]);
	offset += relation->nb_parameters;

	// 6. Scalar.
	OSL_strdup(strings[offset], "1");

	return strings;
}

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

size_t atiling_count_nested_loop(osl_scop_p scop) {
	size_t ret = 0;

	osl_statement_p statement = scop->statement;

	while (statement != NULL) {
		osl_generic_p st_ext = statement->extension;

		while (st_ext != NULL) {
			osl_body_p body = st_ext->data;

			ret = MAX(ret, osl_strings_size(body->iterators));

			st_ext = st_ext->next;
		}

		statement = statement->next;
	}

	return ret;
}

void atiling_get_iterator_name(atiling_loop_p info, osl_statement_p statement) {
	osl_body_p body = statement->extension->data;

	ATILING_strdup(info->it, body->iterators->string[info->depth]);
}

atiling_loop_p atiling_loop_info_get(osl_scop_p scop, size_t index) {
	int parameters_backedup			= 0;
	int arrays_backedup				= 0;
	int iterators_backedup			= 0;
	osl_strings_p parameters_backup = NULL;
	osl_strings_p iterators_backup	= NULL;
	osl_strings_p arrays_backup		= NULL;
	osl_names_p names;

	atiling_loop_p info = calloc(1, sizeof(atiling_loop_t));

	osl_statement_p statement;
	osl_statement_p it_statement = scop->statement;

	int it_len = 0;
	while (it_statement != NULL) {
		osl_generic_p st_ext = it_statement->extension;

		osl_body_p body = st_ext->data;

		if (body != NULL) {
			if (it_len < osl_strings_size(body->iterators)) {
				statement = it_statement;
				it_len	  = osl_strings_size(body->iterators);
			}
		}

		it_statement = it_statement->next;
	}

	if (statement == NULL) {
		atiling_loop_info_free(info);
		return NULL;
	}

	info->depth = index;
	atiling_get_iterator_name(info, statement);

	osl_relation_p domain = statement->domain;

	if (domain->type != OSL_TYPE_DOMAIN) {
		atiling_loop_info_free(info);
		return NULL;
	}
	osl_int_t zero;
	osl_int_init_set_si(domain->precision, &zero, 0);

	for (int i = 0; i < domain->nb_rows; i++) {
		// if m[i][1 + index] > 0
		// ie rel of type loop iterator >= x
		if (osl_int_gt(domain->precision, domain->m[i][1 + index], zero)) {
			info->domain	= domain;
			info->start_row = i;
			info->end_row	= i + 1;
			break;
		}
	}

	osl_int_clear(domain->precision, &zero);

	// Generate names for loop
	names = osl_scop_names(scop);

	// If possible, replace parameter names with scop parameter names.
	if (osl_generic_has_URI(scop->parameters, OSL_URI_STRINGS)) {
		parameters_backedup = 1;
		parameters_backup	= names->parameters;
		names->parameters	= scop->parameters->data;
	}

	// If possible, replace array names with arrays extension names.
	osl_arrays_p arrays = osl_generic_lookup(scop->extension, OSL_URI_ARRAYS);
	if (arrays != NULL) {
		arrays_backedup = 1;
		arrays_backup	= names->arrays;
		names->arrays	= osl_arrays_to_strings(arrays);
	}

	// If possible, replace iterator names with statement iterator
	osl_body_p body =
		(osl_body_p)osl_generic_lookup(statement->extension, OSL_URI_BODY);
	if (body && body->iterators != NULL) {
		iterators_backedup = 1;
		iterators_backup   = names->iterators;
		names->iterators   = body->iterators;
	}

	// link col index with accurate names
	info->name_array = osl_relation_strings(info->domain, names);

	info->parameters_names = osl_strings_malloc();
	unsigned int i		   = 0;
	while (names->parameters->string[i] != NULL) {
		osl_strings_add(info->parameters_names, names->parameters->string[i]);
		i++;
	}

	// If necessary, switch back parameter names.
	if (parameters_backedup) {
		parameters_backedup = 0;
		names->parameters	= parameters_backup;
	}

	// If necessary, switch back array names.
	if (arrays_backedup) {
		arrays_backedup = 0;
		osl_strings_free(names->arrays);
		names->arrays = arrays_backup;
	}

	// If necessary, switch back iterator names.
	if (iterators_backedup) {
		iterators_backedup = 0;
		names->iterators   = iterators_backup;
	}

	osl_names_free(names);
	return info;
}

void atiling_atiling_loop_info_dump_relation(FILE *file, atiling_loop_p info,
											 int row) {

	char *exp = osl_relation_expression(info->domain, row, info->name_array);
	fprintf(file, "%s >=0\n", exp);

	free(exp);
}

void atiling_loop_info_dump(FILE *file, atiling_loop_p info) {
	fprintf(file, "Loop depth %li\n", info->depth);
	fprintf(file, "iterator name = %s\n", info->it);
	fprintf(file, "start :\n\t");
	atiling_atiling_loop_info_dump_relation(file, info, info->start_row);
	fprintf(file, "\n");

	fprintf(file, "end :\n\t");
	atiling_atiling_loop_info_dump_relation(file, info, info->end_row);
}

void atiling_loop_info_free(atiling_loop_p info) {
	if (info) {
		if (info->it != NULL) {
			free(info->it);
		}
		free(info->lb);
		free(info->ub);

		// Free the array of strings.
		if (info->name_array != NULL) {
			for (int i = 0; i < info->domain->nb_columns; i++)
				free(info->name_array[i]);
			free(info->name_array);
		}
		osl_strings_free(info->parameters_names);
		free(info);
	}
}