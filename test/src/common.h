/*
 * Helpers shared by the test programs: sleeping, Ctrl-C, WAV loading.
 */

#ifndef TESTS_COMMON_H
#define TESTS_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <AL/al.h>
#include <AL/alc.h>

#ifdef __GNUC__
#define OAL_UNUSED __attribute__((unused))
#else
#define OAL_UNUSED
#endif

#if defined(__amigaos__) || defined(__VBCC__) || defined(__SASC) || defined(AMIGA)
#include <dos/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

static OAL_UNUSED void sleep_ms(long ms)
{
	long ticks = ms / 20;

	Delay(ticks > 0 ? ticks : 1);
}

static OAL_UNUSED int break_pressed(void)
{
	return (SetSignal(0, SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C) != 0;
}
#else
#include <unistd.h>

static OAL_UNUSED void sleep_ms(long ms)
{
	usleep((useconds_t)ms * 1000);
}

static OAL_UNUSED int break_pressed(void)
{
	return 0;
}
#endif

static int host_is_big_endian(void)
{
	const unsigned short one = 1;
	return *(const unsigned char *)&one == 0;
}

typedef struct {
	void *data;
	ALsizei size;
	ALenum format;
	ALsizei freq;
} wav_data;

static unsigned long rd32(const unsigned char *p)
{
	return p[0] | ((unsigned long)p[1] << 8) | ((unsigned long)p[2] << 16) | ((unsigned long)p[3] << 24);
}

static unsigned rd16(const unsigned char *p)
{
	return p[0] | ((unsigned)p[1] << 8);
}

/* Loads a PCM WAV file (8 or 16 bit, mono or stereo). 16-bit samples are
   converted to the machine's byte order, which is what OpenAL expects. */
static OAL_UNUSED int load_wav(const char *path, wav_data *wav)
{
	FILE *f = fopen(path, "rb");
	unsigned char hdr[12], chunk[8], fmt[16];
	unsigned channels = 0, bits = 0;
	int have_fmt = 0;

	memset(wav, 0, sizeof(*wav));
	if (!f) {
		printf("Cannot open %s\n", path);
		return 0;
	}

	if (fread(hdr, 1, 12, f) != 12 || memcmp(hdr, "RIFF", 4) || memcmp(hdr + 8, "WAVE", 4)) {
		printf("%s is not a WAV file\n", path);
		fclose(f);
		return 0;
	}

	while (fread(chunk, 1, 8, f) == 8) {
		unsigned long len = rd32(chunk + 4);

		if (!memcmp(chunk, "fmt ", 4)) {
			if (len < 16 || fread(fmt, 1, 16, f) != 16)
				break;
			if (rd16(fmt) != 1) {
				printf("Only PCM WAV files are supported\n");
				break;
			}
			channels = rd16(fmt + 2);
			wav->freq = (ALsizei)rd32(fmt + 4);
			bits = rd16(fmt + 14);
			have_fmt = 1;
			fseek(f, (long)(len - 16 + (len & 1)), SEEK_CUR);
		} else if (!memcmp(chunk, "data", 4) && have_fmt) {
			wav->data = malloc(len ? len : 1);
			if (!wav->data)
				break;
			wav->size = (ALsizei)fread(wav->data, 1, len, f);
			break;
		} else {
			fseek(f, (long)(len + (len & 1)), SEEK_CUR);
		}
	}
	fclose(f);

	if (!wav->data) {
		printf("%s: no sound data found\n", path);
		return 0;
	}

	if (channels == 1 && bits == 8) wav->format = AL_FORMAT_MONO8;
	else if (channels == 1 && bits == 16) wav->format = AL_FORMAT_MONO16;
	else if (channels == 2 && bits == 8) wav->format = AL_FORMAT_STEREO8;
	else if (channels == 2 && bits == 16) wav->format = AL_FORMAT_STEREO16;
	else {
		printf("Unsupported WAV format: %u channels, %u bits\n", channels, bits);
		free(wav->data);
		wav->data = NULL;
		return 0;
	}

	if (bits == 16) {
		ALsizei i;
		unsigned char *p = wav->data;

		wav->size &= ~1;
		if (host_is_big_endian()) {
			for (i = 0; i < wav->size; i += 2) {
				unsigned char t = p[i];
				p[i] = p[i + 1];
				p[i + 1] = t;
			}
		}
	}

	return 1;
}

static OAL_UNUSED const char *state_name(ALint state)
{
	switch (state) {
	case AL_INITIAL: return "INITIAL";
	case AL_PLAYING: return "PLAYING";
	case AL_PAUSED:  return "PAUSED";
	case AL_STOPPED: return "STOPPED";
	}
	return "?";
}

#endif
