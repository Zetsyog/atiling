#ifndef ATILING_H
#define ATILING_H

#include "atiling/gen.h"
#include "atiling/loop_info.h"
#include "atiling/macros.h"
#include "atiling/options.h"
#include "atiling/pragma.h"

atiling_pragma_p *atiling_extract(FILE *, atiling_options_p);

#endif