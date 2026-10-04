/*
 * A tone circling the listener, for checking panning, distance and
 * doppler by ear. No files needed.
 *
 *   al_tone3d [SECONDS <n>] [RADIUS <metres>] [SPEED <revolutions per minute>]
 *
 * The tone goes round counter-clockwise seen from above: front, left,
 * behind, right. The circle's centre is half a radius in front of the
 * listener, so the tone also comes closer and goes away again: listen
 * for it moving from the left speaker to the right, and for its pitch
 * being higher while it comes closer (from the front round the left to
 * behind) and lower while it moves away (round the right to the front).
 */

#include "common.h"

#include <math.h>

#define RATE 22050

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

int main(int argc, char **argv)
{
	long seconds = arg_value(argc, argv, "SECONDS", 20);
	long radius = arg_value(argc, argv, "RADIUS", 5);
	long rpm = arg_value(argc, argv, "SPEED", 20);
	ALCdevice *device;
	ALCcontext *ctx;
	ALuint buf, src;
	static ALshort tone[RATE];
	double c, y0, y1, k;
	double px, pz, cz, rc, rs, step_angle;
	long ms, i;
	const long tick_ms = 40;

	/* 441 Hz sine, an exact number of periods in one second so the loop
	   is seamless; made with a resonator so only one cos() is needed */
	k = 2.0 * 3.14159265358979 * 441.0 / RATE;
	c = cos(k);
	y0 = 0.0;
	y1 = -sqrt(1.0 - c * c);   /* sin(-k) */
	for (i = 0; i < RATE; i++) {
		double y = 2.0 * c * y0 - y1;

		tone[i] = (ALshort)(y0 * 12000.0);
		y1 = y0;
		y0 = y;
	}

	device = alcOpenDevice(NULL);
	if (!device) {
		printf("alcOpenDevice failed\n");
		return 10;
	}
	ctx = alcCreateContext(device, NULL);
	alcMakeContextCurrent(ctx);

	alGenBuffers(1, &buf);
	alBufferData(buf, AL_FORMAT_MONO16, tone, sizeof(tone), RATE);
	alGenSources(1, &src);
	alSourcei(src, AL_BUFFER, (ALint)buf);
	alSourcei(src, AL_LOOPING, AL_TRUE);
	alSourcef(src, AL_REFERENCE_DISTANCE, 1.0f);

	/* Rotation per tick, again without sin(): a step of angle a is
	   (cos a, sin a), sin from cos */
	step_angle = 2.0 * 3.14159265358979 * rpm / 60.0 * tick_ms / 1000.0;
	rc = cos(step_angle);
	rs = sqrt(1.0 - rc * rc);
	/* px, pz: position on the circle, relative to its centre */
	cz = -0.5 * radius;
	px = 0.0;
	pz = -(double)radius;

	alSource3f(src, AL_POSITION, (ALfloat)px, 0.0f, (ALfloat)(cz + pz));
	alSourcePlay(src);

	printf("Tone circling at %ld m, %ld rpm, for %ld s (Ctrl-C stops)\n", radius, rpm, seconds);

	for (ms = 0; ms < seconds * 1000 && !break_pressed(); ms += tick_ms) {
		/* Counter-clockwise from above: front (-z) -> left (-x) */
		double nx = px * rc + pz * rs;
		double nz = -px * rs + pz * rc;
		double vx = (nx - px) * 1000.0 / tick_ms;
		double vz = (nz - pz) * 1000.0 / tick_ms;

		px = nx;
		pz = nz;
		alSource3f(src, AL_POSITION, (ALfloat)px, 0.0f, (ALfloat)(cz + pz));
		alSource3f(src, AL_VELOCITY, (ALfloat)vx, 0.0f, (ALfloat)vz);

		if (ms % 1000 == 0) {
			const char *where = "front";

			if (px < -radius * 0.7) where = "left";
			else if (px > radius * 0.7) where = "right";
			else if (cz + pz > 0) where = "behind";
			printf("  %2ld s  %s\n", ms / 1000, where);
		}
		sleep_ms(tick_ms);
	}

	alSourceStop(src);
	alDeleteSources(1, &src);
	alDeleteBuffers(1, &buf);
	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(device);
	return 0;
}
