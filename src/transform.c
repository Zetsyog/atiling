#include <stdint.h>

#include "atiling/atiling.h"
#include "osl/osl.h"
#include "pluto/pluto.h"
#include "pluto/tool.h"

void atiling_apply_transform(atiling_fragment_p frag) {
	PlutoContext *context = pluto_context_alloc();

	// PlutoProg *prog = osl_scop_to_pluto_prog(frag->scop, context);
	// pluto_tile(prog);

	pluto_context_free(context);
}