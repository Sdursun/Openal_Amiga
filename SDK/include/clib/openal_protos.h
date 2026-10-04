#ifndef CLIB_OPENAL_PROTOS_H
#define CLIB_OPENAL_PROTOS_H

/*
 * openal.library: the library's own entry point.
 *
 * Programs normally do not call it: they call OpenAL (alSourcePlay, ...)
 * and link the stub library of the SDK, which opens openal.library and
 * calls through the table OALGetDispatch returns.
 */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

struct OALDispatch;

/* The function table for the given ABI version (OAL_ABI_VERSION in
   <libraries/openal_dispatch.h>), or NULL if the library does not serve
   it. */
const struct OALDispatch *OALGetDispatch(ULONG abi);

#endif
