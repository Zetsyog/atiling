#include "atiling/atiling.h"

atiling_pragma_p *atiling_parse(FILE *input, atiling_options_p options);

atiling_pragma_p *atiling_extract(FILE *input, atiling_options_p options) {
	return atiling_parse(input, options);
}