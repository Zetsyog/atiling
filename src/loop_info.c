
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

size_t count_nested_loop(osl_scop_p scop) {
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

void get_iterator_name(loop_info_p info, osl_statement_p statement) {
	osl_body_p body = statement->extension->data;

	info->name = strdup(body->iterators->string[info->index]);
}

loop_info_p loop_info_get(osl_scop_p scop, size_t index) {
	loop_info_p info = calloc(1, sizeof(loop_info_t));

	osl_statement_p statement = scop->statement;

	while (statement != NULL) {
		osl_generic_p st_ext = statement->extension;

		osl_body_p body = st_ext->data;

		if (body != NULL) {
			if (index < osl_strings_size(body->iterators)) {
				break;
			}
		}

		statement = statement->next;
	}

	if (statement == NULL) {
		loop_info_free(info);
		return NULL;
	}

	info->index = index;
	get_iterator_name(info, statement);
	info->string_names = osl_scop_names(scop);

	// If possible, replace iterator names with statement iterator names.
	osl_body_p body =
		(osl_body_p)osl_generic_lookup(statement->extension, OSL_URI_BODY);
	if (body && body->iterators != NULL) {
		if (info->string_names->iterators != NULL) {
			osl_strings_free(info->string_names->iterators);
		}
		info->string_names->iterators = body->iterators;
	}

	// If possible, replace parameter names with scop parameter names.
	if (osl_generic_has_URI(scop->parameters, OSL_URI_STRINGS)) {
		if (info->string_names->parameters != NULL) {
			osl_strings_free(info->string_names->parameters);
		}
		info->string_names->parameters = scop->parameters->data;
	}

	// If possible, replace array names with arrays extension names.
	osl_arrays_p arrays = osl_generic_lookup(scop->extension, OSL_URI_ARRAYS);
	if (arrays != NULL) {
		if (info->string_names->arrays != NULL) {
			osl_strings_free(info->string_names->arrays);
		}
		info->string_names->arrays = osl_arrays_to_strings(arrays);
	}

	osl_relation_p domain = statement->domain;

	if (domain->type != OSL_TYPE_DOMAIN) {
		loop_info_free(info);
		return NULL;
	}

	osl_int_t zero;
	osl_int_init_set_si(domain->precision, &zero, 0);

	for (int i = 0; i < domain->nb_rows; i++) {
		// if m[i][1 + index] > 0
		// ie rel of type loop iterator >= x
		if (osl_int_gt(domain->precision, domain->m[i][1 + index], zero)) {
			info->relation	= domain;
			info->start_row = i;
			info->end_row	= i + 1;
			break;
		}
	}

	osl_int_clear(domain->precision, &zero);

	return info;
}

void loop_info_dump_relation(FILE *file, loop_info_p info, int row) {
	char **name_array =
		osl_relation_strings(info->relation, info->string_names);

	char *exp = osl_relation_expression(info->relation, row, name_array);
	fprintf(file, "%s >=0\n", exp);

	// Free the array of strings.
	if (name_array != NULL) {
		for (int i = 0; i < info->relation->nb_columns; i++)
			free(name_array[i]);
		free(name_array);
	}
	free(exp);
}

void loop_info_dump(FILE *file, loop_info_p info) {
	fprintf(file, "Loop index %li\n", info->index);
	fprintf(file, "iterator name = %s\n", info->name);
	fprintf(file, "start :\n\t");
	loop_info_dump_relation(file, info, info->start_row);
	fprintf(file, "\n");

	fprintf(file, "end :\n\t");
	loop_info_dump_relation(file, info, info->end_row);
}

void loop_info_free(loop_info_p info) {
	if (info) {
		if (info->name != NULL) {
			free(info->name);
		}
		free(info);
	}
}