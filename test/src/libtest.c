/*
 * Tests openal.library the way two programs use it at once: one library,
 * two handles, each with its own devices, current context and errors.
 * Calls the dispatch table directly, without the stubs. Uses loopback
 * devices only, so it needs no sound hardware (and runs in vamos).
 */

#include <stdio.h>
#include <string.h>
#include <exec/types.h>
#include <exec/libraries.h>
#include <proto/exec.h>

#include <proto/openal.h>

struct Library *OpenALBase;

static int checks, failures;

static void check(int ok, const char *what, int line)
{
	checks++;
	if (!ok) {
		failures++;
		printf("FAIL line %d: %s\n", line, what);
	}
}

#define CHECK(c) check((c) ? 1 : 0, #c, __LINE__)

static ALCint attrs[] = {
	ALC_FREQUENCY, 22050,
	ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT,
	ALC_FORMAT_TYPE_SOFT, ALC_SHORT_SOFT,
	0
};

static ALshort out[512 * 2];

int main(void)
{
	struct Library *base1, *base2;
	const struct OALDispatch *d;
	void *h1, *h2;
	ALCdevice *dev1, *dev2;
	ALCcontext *ctx1, *ctx2;
	ALuint buf, src;
	ALshort data[200];
	ALfloat f = 0.0f;
	ALdouble dbl = 0.0;
	int i;

	base1 = OpenLibrary((STRPTR)"openal.library", 1);
	if (!base1) {
		printf("Cannot open openal.library (put it in LIBS:)\n");
		return 10;
	}
	printf("%s", (const char *)base1->lib_IdString);
	CHECK(base1->lib_Version == 1);

	/* A second opener: the same library */
	base2 = OpenLibrary((STRPTR)"openal.library", 1);
	CHECK(base2 == base1);

	OpenALBase = base1;
	CHECK(OALGetDispatch(OAL_ABI_VERSION + 1) == NULL);
	CHECK(OALGetDispatch(1) == NULL);     /* 0.2 test builds: floats by value */
	d = OALGetDispatch(OAL_ABI_VERSION);
	CHECK(d != NULL);
	if (!d)
		goto out;
	CHECK(d->oal_abi_version == OAL_ABI_VERSION);
	CHECK(d->oal_count == OAL_FUNC_COUNT);
	CHECK(d->oal_struct_size == sizeof(struct OALDispatch));

	h1 = d->oal_create_handle();
	h2 = d->oal_create_handle();
	CHECK(h1 && h2 && h1 != h2);

	/* Each program its own device and current context */
	dev1 = d->alcLoopbackOpenDeviceSOFT(h1, NULL);
	dev2 = d->alcLoopbackOpenDeviceSOFT(h2, NULL);
	ctx1 = d->alcCreateContext(h1, dev1, attrs);
	ctx2 = d->alcCreateContext(h2, dev2, attrs);
	CHECK(ctx1 && ctx2);
	d->alcMakeContextCurrent(h1, ctx1);
	d->alcMakeContextCurrent(h2, ctx2);
	CHECK(d->alcGetCurrentContext(h1) == ctx1);
	CHECK(d->alcGetCurrentContext(h2) == ctx2);

	/* Float and double results through the result pointer */
	f = 100.5f;
	d->alSpeedOfSound(h1, &f);
	f = 0.0f;
	d->alGetFloat(h1, AL_SPEED_OF_SOUND, &f);
	CHECK(f == 100.5f);
	d->alGetFloat(h2, AL_SPEED_OF_SOUND, &f);
	CHECK(f > 343.0f && f < 344.0f);
	d->alGetDouble(h1, AL_SPEED_OF_SOUND, &dbl);
	CHECK(dbl == 100.5);

	/* Sound in program 1 only */
	for (i = 0; i < 200; i += 2) {
		data[i] = 1000;
		data[i + 1] = -2000;
	}
	d->alGenBuffers(h1, 1, &buf);
	d->alBufferData(h1, buf, AL_FORMAT_STEREO16, data, sizeof(data), 22050);
	d->alGenSources(h1, 1, &src);
	d->alSourcei(h1, src, AL_BUFFER, (ALint)buf);
	d->alSourcei(h1, src, AL_LOOPING, AL_TRUE);
	d->alSourcePlay(h1, src);
	CHECK(d->alGetError(h1) == AL_NO_ERROR);

	d->alcRenderSamplesSOFT(h1, dev1, out, 512);
	CHECK(out[100 * 2] == 1000 && out[100 * 2 + 1] == -2000);
	d->alcRenderSamplesSOFT(h2, dev2, out, 512);
	CHECK(out[100 * 2] == 0 && out[100 * 2 + 1] == 0);

	/* Program 2 does not see program 1's names */
	CHECK(d->alIsSource(h2, src) == AL_FALSE);
	CHECK(d->alIsSource(h1, src) == AL_TRUE);

	/* ALC errors without a device are per program */
	d->alcMakeContextCurrent(h1, (ALCcontext *)&f);  /* not a context */
	CHECK(d->alcGetError(h2, NULL) == ALC_NO_ERROR);
	CHECK(d->alcGetError(h1, NULL) == ALC_INVALID_CONTEXT);
	CHECK(d->alcGetCurrentContext(h1) == ctx1);

	/* Program 1 ends without cleaning up: its device goes with it */
	d->oal_destroy_handle(h1);
	CHECK(d->alcGetContextsDevice(h2, ctx1) == NULL);
	CHECK(d->alcGetError(h2, NULL) == ALC_INVALID_CONTEXT);
	CHECK(d->alcGetContextsDevice(h2, ctx2) == dev2);

	/* Program 2 cleans up properly */
	d->alcMakeContextCurrent(h2, NULL);
	d->alcDestroyContext(h2, ctx2);
	CHECK(d->alcCloseDevice(h2, dev2) == ALC_TRUE);
	d->oal_destroy_handle(h2);

out:
	CloseLibrary(base2);
	CloseLibrary(base1);

	printf("%d checks, %d failed\n", checks, failures);
	return failures ? 5 : 0;
}
