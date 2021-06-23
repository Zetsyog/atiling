#ifndef ATILING_MACROS_H
#define ATILING_MACROS_H

#define ATILING_VERSION			 "0.1.0"

#define ATILING_DEBUG			 1
#define ATILING_FALSE			 0
#define ATILING_TRUE			 1

#define ATILING_MAX_FILENAME_LEN 100
#define ATILING_PRAGMA_FILE_NAME "loop"
#define ATILING_PRAGMA_FILE_EXT	 ".tmp"

#define ATILING_GEN_STR_PCMAX	 "_pcmax"
#define ATILING_GEN_STR_EHRHART	 "_Ehrhart"
#define ATILING_GEN_STR_TILEVOL	 "TILE_VOL_L"
#define ATILING_GEN_STR_TRAHRHE	 "trahrhe_"

/*----------------------------------------------------------------------------+
 |                               UTILITY MACROS                               |
 +----------------------------------------------------------------------------*/

#define ATILING_info(msg)                                             \
	do {                                                              \
		fprintf(stderr, "[ATILING] Info: " msg " (%s).\n", __func__); \
	} while (0)

#define ATILING_debug(msg)                                                 \
	do {                                                                   \
		if (ATILING_DEBUG)                                                 \
			fprintf(stderr, "[ATILING] Debug: " msg " (%s).\n", __func__); \
	} while (0)

#define ATILING_debug_call(function_call) \
	do {                                  \
		if (ATILING_DEBUG)                \
			function_call;                \
	} while (0)

#define ATILING_warning(msg)                                             \
	do {                                                                 \
		fprintf(stderr, "[ATILING] Warning: " msg " (%s).\n", __func__); \
	} while (0)

#define ATILING_error(msg)                                             \
	do {                                                               \
		fprintf(stderr, "[ATILING] Error: " msg " (%s).\n", __func__); \
		exit(1);                                                       \
	} while (0)

#define ATILING_malloc(ptr, type, size)           \
	do {                                          \
		if (((ptr) = (type)malloc(size)) == NULL) \
			ATILING_error("memory overflow");     \
	} while (0)

#define ATILING_realloc(ptr, type, size)                \
	do {                                                \
		if (((ptr) = (type)realloc(ptr, size)) == NULL) \
			ATILING_error("memory overflow");           \
	} while (0)

#define ATILING_strdup(destination, source)               \
	do {                                                  \
		if (source != NULL) {                             \
			if (((destination) = strdup(source)) == NULL) \
				ATILING_error("memory overflow");         \
		} else {                                          \
			destination = NULL;                           \
			ATILING_debug("strdup of a NULL string");     \
		}                                                 \
	} while (0)

#define ATILING_max(x, y) ((x) > (y) ? (x) : (y))

#define ATILING_min(x, y) ((x) < (y) ? (x) : (y))

#endif
