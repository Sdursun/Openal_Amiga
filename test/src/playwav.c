/*
 * Plays a WAV file.
 *
 *   al_playwav <file.wav> [LOOP] [STREAM] [PITCH <percent>] [GAIN <percent>]
 *
 * STREAM plays the file the way games play music: in pieces of a quarter
 * second, queued on one source and refilled as they finish. Ctrl-C stops.
 */

#include "common.h"

#define STREAM_BUFFERS 4

static int arg_is(const char *a, const char *b)
{
	while (*a && *b) {
		char x = *a++, y = *b++;

		if (x >= 'a' && x <= 'z') x -= 32;
		if (y >= 'a' && y <= 'z') y -= 32;
		if (x != y)
			return 0;
	}
	return *a == *b;
}

static ALsizei frame_bytes(ALenum format)
{
	switch (format) {
	case AL_FORMAT_MONO8:    return 1;
	case AL_FORMAT_MONO16:   return 2;
	case AL_FORMAT_STEREO8:  return 2;
	default:                 return 4;
	}
}

static void play_static(ALuint src, const wav_data *wav, int loop)
{
	ALuint buf;
	ALint state = AL_PLAYING, offset;
	long seconds = 0;

	alGenBuffers(1, &buf);
	alBufferData(buf, wav->format, wav->data, wav->size, wav->freq);
	alSourcei(src, AL_BUFFER, (ALint)buf);
	alSourcei(src, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
	alSourcePlay(src);

	while (state == AL_PLAYING && !break_pressed()) {
		sleep_ms(100);
		alGetSourcei(src, AL_SOURCE_STATE, &state);
		alGetSourcei(src, AL_SAMPLE_OFFSET, &offset);
		if (offset / wav->freq != seconds) {
			seconds = offset / wav->freq;
			printf("  %ld s\n", seconds);
		}
	}

	alSourceStop(src);
	alSourcei(src, AL_BUFFER, 0);
	alDeleteBuffers(1, &buf);
}

static void play_stream(ALuint src, const wav_data *wav, int loop)
{
	ALuint bufs[STREAM_BUFFERS];
	ALsizei fb = frame_bytes(wav->format);
	ALsizei chunk = (wav->freq / 4) * fb;
	ALsizei pos = 0;
	long underruns = 0, chunks = 0;
	int i, queued = 0;

	alGenBuffers(STREAM_BUFFERS, bufs);

	for (i = 0; i < STREAM_BUFFERS && pos < wav->size; i++) {
		ALsizei n = wav->size - pos < chunk ? wav->size - pos : chunk;

		alBufferData(bufs[i], wav->format, (char *)wav->data + pos, n, wav->freq);
		alSourceQueueBuffers(src, 1, &bufs[i]);
		pos += n;
		queued++;
	}
	alSourcePlay(src);

	while (queued > 0 && !break_pressed()) {
		ALint processed = 0, state;

		sleep_ms(50);
		alGetSourcei(src, AL_BUFFERS_PROCESSED, &processed);

		while (processed-- > 0) {
			ALuint b;

			alSourceUnqueueBuffers(src, 1, &b);
			queued--;

			if (pos >= wav->size && loop)
				pos = 0;
			if (pos < wav->size) {
				ALsizei n = wav->size - pos < chunk ? wav->size - pos : chunk;

				alBufferData(b, wav->format, (char *)wav->data + pos, n, wav->freq);
				alSourceQueueBuffers(src, 1, &b);
				pos += n;
				queued++;
				chunks++;
			}
		}

		/* Ran dry before the next piece was queued: start again */
		alGetSourcei(src, AL_SOURCE_STATE, &state);
		if (state != AL_PLAYING && queued > 0) {
			underruns++;
			alSourcePlay(src);
		}
	}

	alSourceStop(src);
	{
		ALint n = 0;

		alGetSourcei(src, AL_BUFFERS_QUEUED, &n);
		while (n-- > 0) {
			ALuint b;
			alSourceUnqueueBuffers(src, 1, &b);
		}
	}
	alDeleteBuffers(STREAM_BUFFERS, bufs);

	printf("  %ld pieces refilled, %ld underruns\n", chunks, underruns);
}

int main(int argc, char **argv)
{
	ALCdevice *device;
	ALCcontext *ctx;
	ALuint src;
	wav_data wav;
	int loop = 0, stream = 0, i;
	long pitch = 100, gain = 100;
	ALenum err;

	if (argc < 2) {
		printf("Usage: %s <file.wav> [LOOP] [STREAM] [PITCH <percent>] [GAIN <percent>]\n", argv[0]);
		return 5;
	}

	for (i = 2; i < argc; i++) {
		if (arg_is(argv[i], "LOOP"))
			loop = 1;
		else if (arg_is(argv[i], "STREAM"))
			stream = 1;
		else if (arg_is(argv[i], "PITCH") && i + 1 < argc)
			pitch = atol(argv[++i]);
		else if (arg_is(argv[i], "GAIN") && i + 1 < argc)
			gain = atol(argv[++i]);
	}

	if (!load_wav(argv[1], &wav))
		return 10;

	printf("%s: %ld bytes, %ld Hz, format 0x%x\n", argv[1], (long)wav.size, (long)wav.freq, (unsigned)wav.format);

	device = alcOpenDevice(NULL);
	if (!device) {
		printf("alcOpenDevice failed\n");
		return 10;
	}
	ctx = alcCreateContext(device, NULL);
	alcMakeContextCurrent(ctx);

	alGenSources(1, &src);
	alSourcei(src, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcef(src, AL_PITCH, pitch / 100.0f);
	alSourcef(src, AL_GAIN, gain / 100.0f);

	printf("Playing%s%s (Ctrl-C stops)\n", stream ? " as a stream" : "", loop ? ", looped" : "");
	if (stream)
		play_stream(src, &wav, loop);
	else
		play_static(src, &wav, loop);

	err = alGetError();
	if (err != AL_NO_ERROR)
		printf("AL error 0x%x\n", (unsigned)err);

	alDeleteSources(1, &src);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(device);
	free(wav.data);

	printf("Done\n");
	return 0;
}
