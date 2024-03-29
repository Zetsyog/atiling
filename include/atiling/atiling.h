#ifndef ATILING_H
#define ATILING_H

#include "atiling/fragment.h"
#include "atiling/gen.h"
#include "atiling/loop.h"
#include "atiling/macros.h"
#include "atiling/options.h"
#include "atiling/transform.h"

atiling_fragment_p atiling_extract(FILE *, atiling_options_p);

#endif