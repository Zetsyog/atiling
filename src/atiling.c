#include "atiling/atiling.h"

void atiling_parse(FILE *input, atiling_options_p options);

void atiling_extract(FILE *input, atiling_options_p options) {
	atiling_parse(input, options);
}