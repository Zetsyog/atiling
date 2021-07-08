#include <isl/union_map.h>
#include <isl/union_set.h>
#include <pluto/pluto.h>
#include <stdio.h>

int main(int argc, char **argv) {
	PlutoContext *context = pluto_context_alloc();
	PlutoOptions *options = context->options;

	options->tile	   = 1;
	options->parallel  = 1;
	options->debug	   = 0;
	options->moredebug = 0;

	isl_ctx *ctx = isl_ctx_alloc();

	isl_union_set *domains = isl_union_set_read_from_str(
		ctx, "[N, M] -> { [i, j, k] : i >= 0 and -i+N-1 >= 0 and N-1 >= 0 and "
			 "j >= 0 and i-j >= 0 and k >= 0 and -k+M-1 >= 0 and M-1 >= 0 }");
	// isl_union_map *deps = isl_union_map_read_from_str(ctx, "");

	isl_union_map *schedules =
		pluto_transform(isl_union_set_copy(domains), NULL, NULL, NULL, context);

	if (schedules) {
		isl_printer *printer = isl_printer_to_file(ctx, stdout);
		printer				 = isl_printer_print_union_map(printer, schedules);
		printf("\n");
		isl_printer_free(printer);
		isl_union_map_free(schedules);
	}

	isl_ctx_free(ctx);

	pluto_context_free(context);
}
