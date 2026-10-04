#ifndef INLINE_OPENAL_H
#define INLINE_OPENAL_H

/* openal.library calls for gcc */

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

struct OALDispatch;
struct Library;

static __inline const struct OALDispatch *
__OALGetDispatch(struct Library *base, ULONG abi)
{
	register const struct OALDispatch *result __asm("d0");
	register struct Library *a6 __asm("a6") = base;
	register ULONG d0 __asm("d0") = abi;

	__asm volatile ("jsr -30(%%a6)"
	                : "=r" (result)
	                : "r" (a6), "0" (d0)
	                : "d1", "a0", "a1", "fp0", "fp1", "cc", "memory");
	return result;
}

#define OALGetDispatch(abi) __OALGetDispatch(OpenALBase, (abi))

#endif
