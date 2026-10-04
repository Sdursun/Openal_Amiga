/*
 * Automatic tests through a loopback device: no sound hardware needed.
 *
 * Two kinds of checks:
 *   CHECK       behaviour every OpenAL implementation should share
 *               (states, queues, offsets, errors, which side is louder).
 *               Built with -DREF_OPENAL_SOFT against OpenAL Soft these
 *               run unchanged, which tells whether openal-amiga behaves
 *               the way games expect.
 *   CHECK_IMPL  exact sample values of openal-amiga's own mixer (pan
 *               law, interpolation); skipped against OpenAL Soft.
 *
 * Prints one line per failed check and a summary; exit code 0 when all
 * passed. No %f anywhere: libnix's printf gets some floats wrong.
 */

#include "common.h"

#include <AL/alext.h>

static LPALCLOOPBACKOPENDEVICESOFT p_loopback_open;
static LPALCRENDERSAMPLESSOFT p_render;
static ALCdevice *device;

static int checks, failures;

#define OUT_FRAMES 8192
static ALshort out[OUT_FRAMES * 2];

static void check(int ok, const char *what, int line)
{
	checks++;
	if (!ok) {
		failures++;
		printf("FAIL line %d: %s\n", line, what);
	}
}

#define CHECK(c) check((c) ? 1 : 0, #c, __LINE__)

#ifdef REF_OPENAL_SOFT
#define CHECK_IMPL(c) ((void)0)
#else
#define CHECK_IMPL(c) check((c) ? 1 : 0, #c, __LINE__)
#endif

#define NEAR(a, b, tol) ((a) - (b) <= (tol) && (b) - (a) <= (tol))

static void render(ALsizei frames)
{
	p_render(device, out, frames);
}

static ALint get_state(ALuint src)
{
	ALint v = 0;
	alGetSourcei(src, AL_SOURCE_STATE, &v);
	return v;
}

static ALint get_srci(ALuint src, ALenum param)
{
	ALint v = -12345;
	alGetSourcei(src, param, &v);
	return v;
}

static void fill16(ALshort *buf, ALsizei count, ALshort value)
{
	ALsizei i;

	for (i = 0; i < count; i++)
		buf[i] = value;
}

/* A mono 16-bit buffer of the given length and constant value */
static ALuint make_mono(ALsizei frames, ALsizei freq, ALshort value)
{
	ALshort *data = malloc(sizeof(ALshort) * (size_t)frames);
	ALuint buf = 0;

	fill16(data, frames, value);
	alGenBuffers(1, &buf);
	alBufferData(buf, AL_FORMAT_MONO16, data, frames * 2, freq);
	free(data);
	return buf;
}

static ALuint make_stereo(ALsizei frames, ALsizei freq, ALshort l, ALshort r)
{
	ALshort *data = malloc(sizeof(ALshort) * 2 * (size_t)frames);
	ALuint buf = 0;
	ALsizei i;

	for (i = 0; i < frames; i++) {
		data[i * 2] = l;
		data[i * 2 + 1] = r;
	}
	alGenBuffers(1, &buf);
	alBufferData(buf, AL_FORMAT_STEREO16, data, frames * 4, freq);
	free(data);
	return buf;
}

static ALuint make_source(ALuint buf)
{
	ALuint src = 0;

	alGenSources(1, &src);
	if (buf)
		alSourcei(src, AL_BUFFER, (ALint)buf);
	return src;
}

/* A source at the listener, so mono buffers are not attenuated */
static ALuint make_centered_source(ALuint buf)
{
	ALuint src = make_source(buf);

	alSourcei(src, AL_SOURCE_RELATIVE, AL_TRUE);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, 0.0f);
	return src;
}

static void drop(ALuint src, ALuint buf)
{
	alSourceStop(src);
	alDeleteSources(1, &src);
	if (buf)
		alDeleteBuffers(1, &buf);
}

/* --------------------------------------------------------------------- */

static void test_queries(void)
{
	CHECK(alGetString(AL_VERSION) != NULL);
	CHECK(alGetString(AL_EXTENSIONS) != NULL);
	CHECK(alGetProcAddress("alSourcePlay") != NULL);
	CHECK(alGetProcAddress("alNoSuchFunction") == NULL);
	CHECK(alGetEnumValue("AL_PLAYING") == AL_PLAYING);
	CHECK(alGetEnumValue("AL_FORMAT_STEREO16") == AL_FORMAT_STEREO16);
	CHECK(alIsExtensionPresent("AL_EXT_FLOAT32"));
	CHECK(alIsExtensionPresent("al_ext_float32"));
	CHECK(!alIsExtensionPresent("AL_EXT_NOPE"));
	CHECK(alcIsExtensionPresent(device, "ALC_SOFT_loopback"));
	CHECK(alGetInteger(AL_DISTANCE_MODEL) == AL_INVERSE_DISTANCE_CLAMPED);
	CHECK(alGetError() == AL_NO_ERROR);

	/* The specification says AL_INVALID_ENUM; OpenAL Soft reports
	   AL_INVALID_VALUE. Either is fine for programs. */
	alGetInteger(0x1234);
	{
		ALenum err = alGetError();
		CHECK(err == AL_INVALID_ENUM || err == AL_INVALID_VALUE);
	}
	CHECK(alGetError() == AL_NO_ERROR);

	{
		ALCint major = 0, minor = 0, freq = 0;

		alcGetIntegerv(device, ALC_MAJOR_VERSION, 1, &major);
		alcGetIntegerv(device, ALC_MINOR_VERSION, 1, &minor);
		alcGetIntegerv(device, ALC_FREQUENCY, 1, &freq);
		CHECK(major == 1 && minor == 1);
		CHECK(freq == 22050);
	}
}

static void test_buffers(void)
{
	ALuint buf = make_mono(100, 11025, 0);
	ALint v;
	ALshort data[4] = { 0, 0, 0, 0 };

	CHECK(alIsBuffer(buf));
	alGetBufferi(buf, AL_FREQUENCY, &v); CHECK(v == 11025);
	alGetBufferi(buf, AL_BITS, &v);      CHECK(v == 16);
	alGetBufferi(buf, AL_CHANNELS, &v);  CHECK(v == 1);
	alGetBufferi(buf, AL_SIZE, &v);      CHECK(v == 200);
	CHECK(alGetError() == AL_NO_ERROR);

	alBufferData(buf, AL_FORMAT_MONO16, data, 3, 22050);
	CHECK(alGetError() == AL_INVALID_VALUE);

	alBufferData(buf, 0x7777, data, 4, 22050);
	CHECK(alGetError() == AL_INVALID_ENUM);

	alBufferData(buf, AL_FORMAT_STEREO8, data, 4, 8000);
	alGetBufferi(buf, AL_BITS, &v);      CHECK(v == 8);
	alGetBufferi(buf, AL_CHANNELS, &v);  CHECK(v == 2);
	alGetBufferi(buf, AL_SIZE, &v);      CHECK(v == 4);

	alDeleteBuffers(1, &buf);
	CHECK(!alIsBuffer(buf));
	CHECK(alIsBuffer(0));
	CHECK(alGetError() == AL_NO_ERROR);
}

static void test_static_play(void)
{
	ALuint buf = make_stereo(2000, 22050, 1000, -2000);
	ALuint src = make_source(buf);

	CHECK(get_state(src) == AL_INITIAL);
	CHECK(get_srci(src, AL_SOURCE_TYPE) == AL_STATIC);
	CHECK(get_srci(src, AL_BUFFER) == (ALint)buf);

	alSourcePlay(src);
	CHECK(get_state(src) == AL_PLAYING);

	render(1000);
	CHECK_IMPL(out[0] == 1000 && out[1] == -2000);
	CHECK_IMPL(out[999 * 2] == 1000 && out[999 * 2 + 1] == -2000);
	CHECK(out[500 * 2] > 0 && out[500 * 2 + 1] < 0);
	CHECK(get_state(src) == AL_PLAYING);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 1000, 2));
	CHECK(NEAR(get_srci(src, AL_BYTE_OFFSET), 4000, 8));

	render(1100);
	CHECK(get_state(src) == AL_STOPPED);
	/* OpenAL Soft fades out over some 50 frames after the end */
	CHECK_IMPL(out[1050 * 2] == 0 && out[1050 * 2 + 1] == 0);
	CHECK(out[1099 * 2] < 400);
	CHECK(get_srci(src, AL_SAMPLE_OFFSET) == 0);
	CHECK(get_srci(src, AL_BUFFER) == (ALint)buf);

	/* A buffer attached to a source cannot be deleted */
	alDeleteBuffers(1, &buf);
	CHECK(alGetError() == AL_INVALID_OPERATION);
	CHECK(alIsBuffer(buf));

	/* Play again from the start */
	alSourcePlay(src);
	render(100);
	CHECK(get_state(src) == AL_PLAYING);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 100, 2));

	drop(src, buf);
	CHECK(alGetError() == AL_NO_ERROR);
}

static void test_gain(void)
{
	ALuint buf = make_stereo(4000, 22050, 1000, -2000);
	ALuint src = make_source(buf);

	alSourcef(src, AL_GAIN, 0.5f);
	alSourcePlay(src);
	render(500);
	CHECK_IMPL(NEAR(out[10 * 2], 500, 1) && NEAR(out[10 * 2 + 1], -1000, 1));

	/* Listener gain multiplies in; the change is ramped over one block */
	alListenerf(AL_GAIN, 0.5f);
	render(1000);
	CHECK_IMPL(NEAR(out[900 * 2], 250, 1) && NEAR(out[900 * 2 + 1], -500, 1));
	CHECK(out[900 * 2] > 0 && out[900 * 2] < 400);
	alListenerf(AL_GAIN, 1.0f);

	alSourcef(src, AL_GAIN, -1.0f);
	CHECK(alGetError() == AL_INVALID_VALUE);

	drop(src, buf);
}

static void test_resampling(void)
{
	/* 1000 frames at 11025 Hz last 2000 frames at 22050 Hz */
	ALuint buf = make_mono(1000, 11025, 8000);
	ALuint src = make_centered_source(buf);

	alSourcePlay(src);
	render(1985);
	CHECK(get_state(src) == AL_PLAYING);
	render(30);
	CHECK(get_state(src) == AL_STOPPED);
	drop(src, buf);

	/* Pitch 2: 1000 frames at the output rate last 500 */
	buf = make_mono(1000, 22050, 8000);
	src = make_centered_source(buf);
	alSourcef(src, AL_PITCH, 2.0f);
	alSourcePlay(src);
	render(485);
	CHECK(get_state(src) == AL_PLAYING);
	render(30);
	CHECK(get_state(src) == AL_STOPPED);
	drop(src, buf);
}

static void test_looping(void)
{
	ALuint buf = make_mono(100, 22050, 8000);
	ALuint src = make_centered_source(buf);

	alSourcei(src, AL_LOOPING, AL_TRUE);
	alSourcePlay(src);
	render(1000);
	CHECK(get_state(src) == AL_PLAYING);
	CHECK(get_srci(src, AL_SAMPLE_OFFSET) < 100);
	CHECK(out[999 * 2] != 0);

	alSourcei(src, AL_LOOPING, AL_FALSE);
	render(200);
	CHECK(get_state(src) == AL_STOPPED);

	drop(src, buf);
}

static void test_streaming(void)
{
	ALuint bufs[3], got[3] = { 0, 0, 0 };
	ALuint other;
	ALuint src = make_centered_source(0);
	int i;

	for (i = 0; i < 3; i++)
		bufs[i] = make_mono(500, 22050, (ALshort)(1000 * (i + 1)));

	alSourceQueueBuffers(src, 3, bufs);
	CHECK(alGetError() == AL_NO_ERROR);
	CHECK(get_srci(src, AL_SOURCE_TYPE) == AL_STREAMING);
	CHECK(get_srci(src, AL_BUFFERS_QUEUED) == 3);
	CHECK(get_srci(src, AL_BUFFERS_PROCESSED) == 0);

	/* Different format in the same queue */
	other = make_stereo(10, 22050, 0, 0);
	alSourceQueueBuffers(src, 1, &other);
	CHECK(alGetError() == AL_INVALID_OPERATION);
	CHECK(get_srci(src, AL_BUFFERS_QUEUED) == 3);

	/* No unqueueing before anything was played */
	alSourceUnqueueBuffers(src, 1, got);
	CHECK(alGetError() == AL_INVALID_VALUE);

	alSourcePlay(src);
	render(1200);
	CHECK(get_state(src) == AL_PLAYING);
	CHECK(get_srci(src, AL_BUFFERS_PROCESSED) == 2);
	CHECK(get_srci(src, AL_BUFFER) == (ALint)bufs[2]);

	/* A playing streaming source does not take AL_BUFFER */
	alSourcei(src, AL_BUFFER, (ALint)other);
	CHECK(alGetError() == AL_INVALID_OPERATION);

	alSourceUnqueueBuffers(src, 3, got);
	CHECK(alGetError() == AL_INVALID_VALUE);
	alSourceUnqueueBuffers(src, 2, got);
	CHECK(alGetError() == AL_NO_ERROR);
	CHECK(got[0] == bufs[0] && got[1] == bufs[1]);
	CHECK(get_srci(src, AL_BUFFERS_QUEUED) == 1);
	CHECK(get_srci(src, AL_BUFFERS_PROCESSED) == 0);

	/* Requeue one while playing, as a streaming program does */
	alSourceQueueBuffers(src, 1, &bufs[0]);
	CHECK(get_srci(src, AL_BUFFERS_QUEUED) == 2);
	render(400);
	CHECK(get_srci(src, AL_BUFFERS_PROCESSED) == 1);
	render(600);
	CHECK(get_state(src) == AL_STOPPED);
	CHECK(get_srci(src, AL_BUFFERS_PROCESSED) == 2);

	alSourceUnqueueBuffers(src, 2, got);
	CHECK(alGetError() == AL_NO_ERROR);
	CHECK(got[0] == bufs[2] && got[1] == bufs[0]);
	CHECK(get_srci(src, AL_BUFFERS_QUEUED) == 0);
	CHECK(get_srci(src, AL_SOURCE_TYPE) == AL_STREAMING);

	drop(src, 0);
	alDeleteBuffers(3, bufs);
	alDeleteBuffers(1, &other);
	CHECK(alGetError() == AL_NO_ERROR);
}

static void test_offsets(void)
{
	ALuint buf = make_mono(22050, 22050, 8000);
	ALuint src = make_centered_source(buf);
	ALfloat sec = 0.0f;

	/* Set before Play: applied when playback starts */
	alSourcef(src, AL_SEC_OFFSET, 0.5f);
	CHECK(alGetError() == AL_NO_ERROR);
	alSourcePlay(src);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 11025, 2));
	render(100);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 11125, 2));

	/* Set while playing: applied at once */
	alSourcei(src, AL_BYTE_OFFSET, 2000);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 1000, 2));
	alGetSourcef(src, AL_SEC_OFFSET, &sec);
	CHECK(sec > 0.044f && sec < 0.047f);

	/* Past the end */
	alSourcei(src, AL_SAMPLE_OFFSET, 50000);
	CHECK(alGetError() == AL_INVALID_VALUE);

	drop(src, buf);
}

static void test_states(void)
{
	ALuint buf, src = make_source(0);

	/* Play without a buffer stops at once */
	alSourcePlay(src);
	CHECK(get_state(src) == AL_STOPPED);
	alSourceRewind(src);
	CHECK(get_state(src) == AL_INITIAL);

	/* Stop and Pause leave an initial source alone */
	alSourceStop(src);
	CHECK(get_state(src) == AL_INITIAL);
	alSourcePause(src);
	CHECK(get_state(src) == AL_INITIAL);
	alDeleteSources(1, &src);

	/* Pause keeps the position */
	buf = make_mono(5000, 22050, 8000);
	src = make_centered_source(buf);
	alSourcePlay(src);
	render(1000);
	alSourcePause(src);
	CHECK(get_state(src) == AL_PAUSED);
	render(1000);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 1000, 2));
	CHECK(out[500 * 2] == 0);
	alSourcePlay(src);
	CHECK(get_state(src) == AL_PLAYING);
	render(500);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 1500, 2));

	/* Stop, then Play starts from the beginning */
	alSourceStop(src);
	CHECK(get_state(src) == AL_STOPPED);
	alSourcePlay(src);
	CHECK(get_srci(src, AL_SAMPLE_OFFSET) == 0);

	/* Rewind while playing */
	render(200);
	alSourceRewind(src);
	CHECK(get_state(src) == AL_INITIAL);
	CHECK(get_srci(src, AL_SAMPLE_OFFSET) == 0);

	/* Playv / Stopv with an invalid name do nothing */
	{
		ALuint list[2];

		list[0] = src;
		list[1] = 0xDEAD;
		alSourcePlayv(2, list);
		CHECK(alGetError() == AL_INVALID_NAME);
		CHECK(get_state(src) == AL_INITIAL);
	}

	drop(src, buf);
	CHECK(alGetError() == AL_NO_ERROR);
}

/* Vectors with fractions survive a set and get through every form. gcc
   6.5 with -m68040 once stored y and z of alSource3f as garbage (whole
   numbers happened to come out as 0, fractions as anything). */
static void test_vectors(void)
{
	ALuint src = make_source(0);
	ALenum params[3] = { AL_POSITION, AL_VELOCITY, AL_DIRECTION };
	ALfloat fv[3], x, y, z;
	ALint iv[3];
	int p;

	for (p = 0; p < 3; p++) {
		alSource3f(src, params[p], 3.25f, -4.125f, 0.7f);
		alGetSource3f(src, params[p], &x, &y, &z);
		CHECK(x == 3.25f && y == -4.125f && z == 0.7f);

		fv[0] = -1.5f; fv[1] = 2.75f; fv[2] = -0.3f;
		alSourcefv(src, params[p], fv);
		fv[0] = fv[1] = fv[2] = 0.0f;
		alGetSourcefv(src, params[p], fv);
		CHECK(fv[0] == -1.5f && fv[1] == 2.75f && fv[2] == -0.3f);

		alSource3i(src, params[p], 7, -8, 9);
		alGetSourceiv(src, params[p], iv);
		CHECK(iv[0] == 7 && iv[1] == -8 && iv[2] == 9);
	}

	alListener3f(AL_POSITION, 0.5f, -1.25f, 2.2f);
	alGetListener3f(AL_POSITION, &x, &y, &z);
	CHECK(x == 0.5f && y == -1.25f && z == 2.2f);
	alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f);

	alDeleteSources(1, &src);
	CHECK(alGetError() == AL_NO_ERROR);
}

static void test_errors(void)
{
	ALuint src = make_source(0);
	ALint v;

	alSourcei(src, AL_BUFFER, 9999);
	CHECK(alGetError() == AL_INVALID_VALUE);

	alGetSourcei(0xDEAD, AL_GAIN, &v);
	CHECK(alGetError() == AL_INVALID_NAME);

	alSourcef(src, 0x7777, 1.0f);
	CHECK(alGetError() == AL_INVALID_ENUM);

	/* Only the first error is kept */
	alSourcef(src, AL_GAIN, -1.0f);
	alSourcef(src, 0x7777, 1.0f);
	CHECK(alGetError() == AL_INVALID_VALUE);
	CHECK(alGetError() == AL_NO_ERROR);

	/* Read-only properties */
	alSourcei(src, AL_SOURCE_STATE, AL_PLAYING);
	CHECK(alGetError() != AL_NO_ERROR);

	/* Wrong number of values */
	alSource3f(src, AL_GAIN, 1.0f, 1.0f, 1.0f);
	CHECK(alGetError() == AL_INVALID_ENUM);

	alDeleteSources(1, &src);
	CHECK(!alIsSource(src));
	alDeleteSources(1, &src);
	CHECK(alGetError() == AL_INVALID_NAME);
}

/* Average of one channel over the last frames rendered */
static long average(ALsizei from, ALsizei to, int channel)
{
	long sum = 0;
	ALsizei i;

	for (i = from; i < to; i++)
		sum += out[i * 2 + channel];
	return sum / (to - from);
}

static void test_panning(void)
{
	ALuint buf = make_mono(22050, 22050, 10000);
	ALuint src = make_source(buf);
	long l, r;
	ALfloat ori[6];

	alDistanceModel(AL_NONE);
	alSourcei(src, AL_LOOPING, AL_TRUE);

	/* To the right */
	alSource3f(src, AL_POSITION, 10.0f, 0.0f, 0.0f);
	alSourcePlay(src);
	render(1024);
	l = average(512, 1024, 0);
	r = average(512, 1024, 1);
	CHECK(r > 4 * l + 1000);
	CHECK(NEAR(r, 10000, 3) && NEAR(l, 0, 3));

	/* To the left */
	alSource3f(src, AL_POSITION, -10.0f, 0.0f, 0.0f);
	render(1024);
	l = average(512, 1024, 0);
	r = average(512, 1024, 1);
	CHECK(l > 4 * r + 1000);

	/* In front: both sides the same */
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -10.0f);
	render(1024);
	l = average(512, 1024, 0);
	r = average(512, 1024, 1);
	CHECK(NEAR(l, r, 5) && NEAR(l, 5956, 5));

	/* Behind: quieter than in front */
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, 10.0f);
	render(1024);
	CHECK(NEAR(average(512, 1024, 0), 4043, 5) && NEAR(average(512, 1024, 1), 4043, 5));

	/* Above */
	alSource3f(src, AL_POSITION, 0.0f, 10.0f, 0.0f);
	render(1024);
	CHECK(NEAR(average(512, 1024, 0), 5000, 5) && NEAR(average(512, 1024, 1), 5000, 5));

	/* 30 degrees to the right */
	alSource3f(src, AL_POSITION, 5.0f, 0.0f, -8.660254f);
	render(1024);
	CHECK(NEAR(average(512, 1024, 0), 2140, 5) && NEAR(average(512, 1024, 1), 9211, 5));

	/* Listener turned to look along +x: +z is now on the right */
	ori[0] = 1.0f; ori[1] = 0.0f; ori[2] = 0.0f;
	ori[3] = 0.0f; ori[4] = 1.0f; ori[5] = 0.0f;
	alListenerfv(AL_ORIENTATION, ori);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, 10.0f);
	render(1024);
	l = average(512, 1024, 0);
	r = average(512, 1024, 1);
	CHECK(r > 4 * l + 1000);

	ori[0] = 0.0f; ori[2] = -1.0f;
	alListenerfv(AL_ORIENTATION, ori);
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	drop(src, buf);
	CHECK(alGetError() == AL_NO_ERROR);
}

static void test_distance(void)
{
	ALuint buf = make_mono(22050, 22050, 10000);
	ALuint src = make_source(buf);
	long near_l, far_l;

	alSourcei(src, AL_LOOPING, AL_TRUE);

	/* Inverse clamped, reference 1, rolloff 1: gain 1/d */
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -1.0f);
	alSourcePlay(src);
	render(1024);
	near_l = average(512, 1024, 0);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -2.0f);
	render(1024);
	far_l = average(512, 1024, 0);
	CHECK(far_l < near_l * 6 / 10 && far_l > near_l * 4 / 10);
	CHECK(NEAR(far_l, 2978, 4));

	/* Linear clamped: halfway between reference and maximum */
	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
	alSourcef(src, AL_MAX_DISTANCE, 11.0f);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -6.0f);
	render(1024);
	CHECK(NEAR(average(512, 1024, 0), 2978, 4));

	/* Beyond the maximum distance: silent */
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -20.0f);
	render(1024);
	CHECK(average(512, 1024, 0) == 0);

	/* Exponent: (d / ref) ^ -rolloff */
	alDistanceModel(AL_EXPONENT_DISTANCE);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -4.0f);
	render(1024);
	CHECK(NEAR(average(512, 1024, 0), 1489, 4));

	/* min / max gain clamp the attenuated gain */
	alSourcef(src, AL_MIN_GAIN, 0.5f);
	render(1024);
	CHECK(NEAR(average(512, 1024, 0), 2978, 4));

	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	drop(src, buf);
	CHECK(alGetError() == AL_NO_ERROR);
}

static void test_cone(void)
{
	ALuint buf = make_mono(22050, 22050, 10000);
	ALuint src = make_source(buf);

	/* In front of the listener, pointing away from it */
	alDistanceModel(AL_NONE);
	alSourcei(src, AL_LOOPING, AL_TRUE);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -10.0f);
	alSource3f(src, AL_DIRECTION, 0.0f, 0.0f, -1.0f);
	alSourcef(src, AL_CONE_INNER_ANGLE, 90.0f);
	alSourcef(src, AL_CONE_OUTER_ANGLE, 180.0f);
	alSourcef(src, AL_CONE_OUTER_GAIN, 0.0f);
	alSourcePlay(src);
	render(1024);
	CHECK(average(512, 1024, 0) == 0);

	/* Pointing at the listener */
	alSource3f(src, AL_DIRECTION, 0.0f, 0.0f, 1.0f);
	render(1024);
	CHECK(average(512, 1024, 0) > 3000);

	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	drop(src, buf);
}

static void test_doppler(void)
{
	ALuint buf = make_mono(22050, 22050, 8000);
	ALuint src = make_source(buf);
	ALint off;

	/* Moving towards the listener at a tenth of the speed of sound:
	   the pitch goes up by 1 / 0.9 */
	alDistanceModel(AL_NONE);
	alSource3f(src, AL_POSITION, 0.0f, 0.0f, -100.0f);
	alSource3f(src, AL_VELOCITY, 0.0f, 0.0f, 34.33f);
	alSourcePlay(src);
	render(900);
	off = get_srci(src, AL_SAMPLE_OFFSET);
	CHECK(NEAR(off, 1000, 15));

	/* Doppler off */
	alDopplerFactor(0.0f);
	alSourcePlay(src);
	render(900);
	CHECK(NEAR(get_srci(src, AL_SAMPLE_OFFSET), 900, 3));

	alDopplerFactor(1.0f);
	alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
	drop(src, buf);
}

static void test_formats(void)
{
	ALuint buf, src;
	unsigned char s8[4] = { 192, 64, 192, 64 };
	ALfloat f32[4] = { 0.5f, -0.25f, 0.5f, -0.25f };

	alGenBuffers(1, &buf);
	alBufferData(buf, AL_FORMAT_STEREO8, s8, 4, 22050);
	src = make_source(buf);
	alSourcei(src, AL_LOOPING, AL_TRUE);
	alSourcePlay(src);
	/* Look past OpenAL Soft's fade-in */
	render(512);
	CHECK(out[400 * 2] > 15000 && out[400 * 2 + 1] < -15000);
	CHECK_IMPL(out[400 * 2] == 16384 && out[400 * 2 + 1] == -16384);
	drop(src, buf);

	alGenBuffers(1, &buf);
	alBufferData(buf, AL_FORMAT_STEREO_FLOAT32, f32, sizeof(f32), 22050);
	CHECK(alGetError() == AL_NO_ERROR);
	src = make_source(buf);
	alSourcei(src, AL_LOOPING, AL_TRUE);
	alSourcePlay(src);
	render(512);
	CHECK(NEAR(out[400 * 2], 16383, 2) && NEAR(out[400 * 2 + 1], -8191, 2));
	drop(src, buf);
}

static void test_many_sources(void)
{
	ALuint buf = make_mono(2000, 22050, 100);
	ALuint src[64];
	int i, playing = 0;

	alGenSources(64, src);
	CHECK(alGetError() == AL_NO_ERROR);

	for (i = 0; i < 64; i++) {
		alSourcei(src[i], AL_BUFFER, (ALint)buf);
		alSourcei(src[i], AL_SOURCE_RELATIVE, AL_TRUE);
	}
	alSourcePlayv(64, src);
	render(1000);

	for (i = 0; i < 64; i++)
		playing += get_state(src[i]) == AL_PLAYING;
	CHECK(playing == 64);

	alSourceStopv(64, src);
	alDeleteSources(64, src);
	alDeleteBuffers(1, &buf);
	CHECK(alGetError() == AL_NO_ERROR);
}

int main(void)
{
	ALCint attrs[] = {
		ALC_FREQUENCY, 22050,
		ALC_FORMAT_CHANNELS_SOFT, ALC_STEREO_SOFT,
		ALC_FORMAT_TYPE_SOFT, ALC_SHORT_SOFT,
		0
	};
	ALCcontext *ctx;

	p_loopback_open = (LPALCLOOPBACKOPENDEVICESOFT)alcGetProcAddress(NULL, "alcLoopbackOpenDeviceSOFT");
	p_render = (LPALCRENDERSAMPLESSOFT)alcGetProcAddress(NULL, "alcRenderSamplesSOFT");
	if (!p_loopback_open || !p_render) {
		printf("No ALC_SOFT_loopback\n");
		return 1;
	}

	device = p_loopback_open(NULL);
	if (!device) {
		printf("Cannot open a loopback device\n");
		return 1;
	}

	ctx = alcCreateContext(device, attrs);
	if (!ctx || !alcMakeContextCurrent(ctx)) {
		printf("Cannot create a context\n");
		return 1;
	}

	printf("Testing %s (%s)\n", alGetString(AL_RENDERER), alGetString(AL_VERSION));

	test_queries();
	test_buffers();
	test_static_play();
	test_gain();
	test_resampling();
	test_looping();
	test_streaming();
	test_offsets();
	test_states();
	test_vectors();
	test_errors();
	test_panning();
	test_distance();
	test_cone();
	test_doppler();
	test_formats();
	test_many_sources();

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	CHECK(alcCloseDevice(device));

	printf("%d checks, %d failed\n", checks, failures);
	return failures ? 1 : 0;
}
