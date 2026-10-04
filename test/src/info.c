/*
 * Opens the default device and prints what it reports. The first thing
 * to run on new hardware: it shows whether AHI could be opened at all.
 */

#include "common.h"

static void print_list(const char *title, const ALCchar *list)
{
	printf("%s:\n", title);
	if (!list) {
		printf("  (none)\n");
		return;
	}
	while (*list) {
		printf("  %s\n", list);
		list += strlen(list) + 1;
	}
}

int main(void)
{
	ALCdevice *device;
	ALCcontext *ctx;
	ALCint size = 0, attrs[32];
	int i;

	print_list("Devices", alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	printf("Default device: %s\n", alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER));

	device = alcOpenDevice(NULL);
	if (!device) {
		printf("alcOpenDevice failed (ALC error 0x%x).\n", (unsigned)alcGetError(NULL));
		printf("Is AHI installed, and does the AHI unit (ENV:OpenAL/Unit) have a mode set?\n");
		return 10;
	}

	printf("Opened: %s\n", alcGetString(device, ALC_DEVICE_SPECIFIER));
	printf("ALC extensions: %s\n", alcGetString(device, ALC_EXTENSIONS));

	ctx = alcCreateContext(device, NULL);
	if (!ctx) {
		printf("alcCreateContext failed\n");
		alcCloseDevice(device);
		return 10;
	}
	alcMakeContextCurrent(ctx);

	alcGetIntegerv(device, ALC_ATTRIBUTES_SIZE, 1, &size);
	if (size > 0 && size <= 32) {
		alcGetIntegerv(device, ALC_ALL_ATTRIBUTES, size, attrs);
		for (i = 0; i + 1 < size && attrs[i]; i += 2) {
			const char *name = "?";

			switch (attrs[i]) {
			case ALC_FREQUENCY:      name = "Frequency"; break;
			case ALC_REFRESH:        name = "Refresh"; break;
			case ALC_SYNC:           name = "Sync"; break;
			case ALC_MONO_SOURCES:   name = "Mono sources"; break;
			case ALC_STEREO_SOURCES: name = "Stereo sources"; break;
			}
			printf("  %-15s %ld\n", name, (long)attrs[i + 1]);
		}
	}

	printf("AL vendor:     %s\n", alGetString(AL_VENDOR));
	printf("AL renderer:   %s\n", alGetString(AL_RENDERER));
	printf("AL version:    %s\n", alGetString(AL_VERSION));
	printf("AL extensions: %s\n", alGetString(AL_EXTENSIONS));

	alcMakeContextCurrent(NULL);
	alcDestroyContext(ctx);
	alcCloseDevice(device);

	printf("OK\n");
	return 0;
}
