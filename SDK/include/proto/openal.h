#ifndef PROTO_OPENAL_H
#define PROTO_OPENAL_H

/* openal.library for gcc, vbcc and SAS/C */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif
#ifndef LIBRARIES_OPENAL_DISPATCH_H
#include <libraries/openal_dispatch.h>
#endif

extern struct Library *OpenALBase;

#include <clib/openal_protos.h>

#if defined(__VBCC__)
#include <inline/openal_protos.h>
#elif defined(__GNUC__)
#include <inline/openal.h>
#elif defined(__SASC)
#include <pragmas/openal_pragmas.h>
#endif

#endif
