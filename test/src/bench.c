/*
 * Measures the mixer: renders sound for many moving 3D sources through a
 * loopback device, as fast as possible, and reports how much of the CPU
 * the same mix would take in real time.
 *
 *   al_bench [SOURCES <n>] [SECONDS <n>] [RATE <Hz>]
 */

#include "common.h"

#include <AL/alext.h>

#if defined(__amigaos__) || defined(__VBCC__) || defined(__SASC) || defined(AMIGA)
#include <dos/dos.h>

static long now_ms(void)
{
	struct DateStamp ds;

	DateStamp(&ds);
	return ds.ds_Days * 86400000L + ds.ds_Minute * 60000L + ds.ds_Tick * 20L;
}
#else
#include <time.h>

static long now_ms(void)
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000L + ts.tv_nsec / 1000000L;
}
#endif

static long arg_value(int argc, char **argv, const char *name, long def)
{
	int i;

	for (i = 1; i + 1 < argc; i++) {
		const char *a = argv[i], *b = name;

		while (*a && *b && ((*a | 32) == (*b | 32))) {
			a++;
			b++;
		}
		if (!*a && !*b)
			return atol(argv[i + 1]);
	}
	return def;
}

#define BLOCK 1024

int main(int argc, char **argv)
{
	long nsrc = arg_value(argc, argv, "SOURCES", 32);
	long seconds = arg_value(argc, argv, "SECONDS", 10);
	long rate = arg_value(argc, argv, "RATE", 22050);
	LPALCLOOPBACKOPENDEVICESOFT p_open;
	LPALCRENDERSAMPLESSOFT p_render;
	ALCdevice *device;
	ALCcontext *ctx;
	ALCint attrs[7];
	static ALshort noise[11025], out[BLOCK * 2];
	ALuint buf, *src;
	unsigned long seed = 1;
	long i, frames, done, t0, t1, ms;

	p_open = (LPALCLOOPBACKOPENDEVICESOFT)alcGetProcAddress(NULL, "alcLoopbackOpenDeviceSOFT");
	p_render = (LPALCRENDERSAMPLESSOFT)alcGetProcAddress(NULL, "alcRenderSamplesSOFT");
	if (!p_open || !p_render)
		return 10;

	attrs[0] = ALC_FREQUENCY;            attrs[1] = (ALCint)rate;
	attrs[2] = ALC_FORMAT_CHANNELS_SOFT; attrs[3] = ALC_STEREO_SOFT;
	attrs[4] = ALC_FORMAT_TYPE_SOFT;     attrs[5] = ALC_SHORT_SOFT;
	attrs[6] = 0;

	device = p_open(NULL);
	ctx = device ? alcCreateContext(device, attrs) : NULL;
	if (!ctx) {
		printf("Cannot create a loopback context\n");
		return 10;
	}
	alcMakeContextCurrent(ctx);

	for (i = 0; i < 11025; i++) {
		seed = seed * 1103515245UL + 12345UL;
		noise[i] = (ALshort)((long)((seed >> 16) & 0x7FFF) - 16384);
	}

	/* 11025 Hz samples, as games mostly have: every source resamples */
	alGenBuffers(1, &buf);
	alBufferData(buf, AL_FORMAT_MONO16, noise, sizeof(noise), 11025);

	src = malloc(sizeof(ALuint) * (size_t)nsrc);
	alGenSources((ALsizei)nsrc, src);
	for (i = 0; i < nsrc; i++) {
		alSourcei(src[i], AL_BUFFER, (ALint)buf);
		alSourcei(src[i], AL_LOOPING, AL_TRUE);
		alSource3f(src[i], AL_POSITION, (ALfloat)(i % 7) - 3.0f, 0.0f, -(ALfloat)(i % 5) - 1.0f);
		alSource3f(src[i], AL_VELOCITY, 1.0f, 0.0f, 0.5f);
	}
	alSourcePlayv((ALsizei)nsrc, src);

	frames = seconds * rate;
	t0 = now_ms();
	for (done = 0; done < frames; done += BLOCK) {
		/* Move the sources a little, as a game does every frame */
		if ((done / BLOCK) % 2 == 0) {
			for (i = 0; i < nsrc; i++)
				alSource3f(src[i], AL_POSITION, (ALfloat)((done / BLOCK + i) % 11) - 5.0f, 0.0f, -2.0f);
		}
		p_render(device, out, BLOCK);
	}
	t1 = now_ms();
	ms = t1 - t0;
	if (ms <= 0)
		ms = 1;

	printf("%ld sources, %ld s of sound at %ld Hz mixed in %ld ms\n", nsrc, seconds, rate, ms);
	printf("CPU time in real time: %ld.%ld%%\n", ms / (seconds * 10), (ms * 10 / (seconds * 10)) % 10);

	alSourceStopv((ALsizei)nsrc, src);
	alDeleteSources((ALsizei)nsrc, src);
	alDeleteBuffers(1, &buf);
	free(src);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(device);
	return 0;
}
