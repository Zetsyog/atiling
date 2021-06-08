#include "parser.h"
#include <stdlib.h>
#include <string.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))

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

		if (index < osl_strings_size(body->iterators)) {
			break;
		}

		statement = statement->next;
	}

	if (statement == NULL) {
		loop_info_free(info);
		return NULL;
	}

	info->index = index;
	get_iterator_name(info, statement);

	osl_relation_p domain = statement->domain;

	if (domain->type != OSL_TYPE_DOMAIN) {
		loop_info_free(info);
		return NULL;
	}

	info->precision = domain->precision;

	osl_int_t zero;
	osl_int_init_set_si(domain->precision, &zero, 0);

	for (int i = 0; i < domain->nb_rows; i++) {
		if (osl_int_gt(domain->precision, domain->m[i][1 + index], zero)) {
			printf("size=%i\n", domain->nb_columns);
			info->start_len = domain->nb_columns - 1;
			info->start		= malloc(sizeof(osl_int_t) * info->start_len);

			for (int j = 1; j < domain->nb_columns; j++) {
				osl_int_init_set(domain->precision, &info->start[j - 1],
								 domain->m[i][j]);
			}
		}

		for (int j = 0; j < domain->nb_columns; j++) {
			osl_int_print(stdout, 0, domain->m[i][j]);
			printf(" ");
		}
		printf("\n");
	}

	osl_int_clear(domain->precision, &zero);

	return info;
}

void loop_info_dump(loop_info_p info) {
	printf("Loop index %li\n", info->index);
	printf("iterator name = %s\n", info->name);
	printf("start (%i) :\n\t", info->start_len);
	for (int i = 0; i < info->start_len; i++) {
		osl_int_print(stdout, info->precision, info->start[i]);
		printf(" ");
	}
	printf("\n");
}

void loop_info_free(loop_info_p info) {
	if (info) {
		if (info->name != NULL) {
			free(info->name);
		}
		free(info);
	}
}
