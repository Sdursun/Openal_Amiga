#ifndef _VBCCINLINE_OPENAL_H
#define _VBCCINLINE_OPENAL_H

/* openal.library calls for vbcc */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

struct OALDispatch;
struct Library;

const struct OALDispatch *__OALGetDispatch(__reg("a6") struct Library *, __reg("d0") ULONG abi) = "\tjsr\t-30(a6)";
#define OALGetDispatch(abi) __OALGetDispatch(OpenALBase, (abi))

#endif
