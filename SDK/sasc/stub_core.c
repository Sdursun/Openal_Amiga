/*
 * The program side of openal.library: opens the library on the first
 * OpenAL call and closes it when the program exits.
 *
 * Programs link the SDK's stub library instead of the static OpenAL and
 * call OpenAL as usual. Without openal.library every call does nothing
 * and alcOpenDevice returns NULL, so a game runs on without sound.
 *
 * Plain C for gcc, vbcc and SAS/C; <proto/openal.h> picks the way each
 * compiler calls the library.
 */

#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>
#include <stdlib.h>

#include <proto/openal.h>

struct Library *OpenALBase;
const struct OALDispatch *oal_stub_d;
void *oal_stub_h;

static int tried;

static void stub_exit(void)
{
	if (oal_stub_d) {
		oal_stub_d->oal_destroy_handle(oal_stub_h);
		oal_stub_d = NULL;
		oal_stub_h = NULL;
	}
	if (OpenALBase) {
		CloseLibrary(OpenALBase);
		OpenALBase = NULL;
	}
}

int oal_stub_init(void)
{
	const struct OALDispatch *d;
	void *h;

	if (tried)
		return oal_stub_d != NULL;
	tried = 1;

	OpenALBase = OpenLibrary((STRPTR)"openal.library", 1);
	if (!OpenALBase)
		return 0;

	d = OALGetDispatch(OAL_ABI_VERSION);

	/* A library older than this program lacks functions it may call */
	if (!d || d->oal_abi_version != OAL_ABI_VERSION || d->oal_count < OAL_FUNC_COUNT) {
		CloseLibrary(OpenALBase);
		OpenALBase = NULL;
		return 0;
	}

	h = d->oal_create_handle();
	if (!h) {
		CloseLibrary(OpenALBase);
		OpenALBase = NULL;
		return 0;
	}

	oal_stub_h = h;
	oal_stub_d = d;
	atexit(stub_exit);
	return 1;
}
