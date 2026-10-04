/* openal.library SDK: OpenAL functions that call the library. Plain C,
   for any Amiga compiler; built into SDK/gcc/libopenalshared.a and
   SDK/vbcc/openal.lib, and here for SAS/C (see smakefile). */

#include <string.h>
#include <libraries/openal_dispatch.h>

extern const struct OALDispatch *oal_stub_d;
extern void *oal_stub_h;
int oal_stub_init(void);

/* Opens openal.library on first use; without it every call does nothing */
#define READY (oal_stub_d || oal_stub_init())

ALCcontext* alcCreateContext(ALCdevice *device, const ALCint *attrlist)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcCreateContext(oal_stub_h, device, attrlist);
}

ALCboolean alcMakeContextCurrent(ALCcontext *context)
{
	if (!READY)
		return 0;
	return (ALCboolean)oal_stub_d->alcMakeContextCurrent(oal_stub_h, context);
}

void alcProcessContext(ALCcontext *context)
{
	if (READY)
		oal_stub_d->alcProcessContext(oal_stub_h, context);
}

void alcSuspendContext(ALCcontext *context)
{
	if (READY)
		oal_stub_d->alcSuspendContext(oal_stub_h, context);
}

void alcDestroyContext(ALCcontext *context)
{
	if (READY)
		oal_stub_d->alcDestroyContext(oal_stub_h, context);
}

ALCcontext* alcGetCurrentContext(void)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcGetCurrentContext(oal_stub_h);
}

ALCdevice* alcGetContextsDevice(ALCcontext *context)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcGetContextsDevice(oal_stub_h, context);
}

ALCdevice* alcOpenDevice(const ALCchar *devicename)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcOpenDevice(oal_stub_h, devicename);
}

ALCboolean alcCloseDevice(ALCdevice *device)
{
	if (!READY)
		return 0;
	return (ALCboolean)oal_stub_d->alcCloseDevice(oal_stub_h, device);
}

ALCenum alcGetError(ALCdevice *device)
{
	if (!READY)
		return 0;
	return oal_stub_d->alcGetError(oal_stub_h, device);
}

ALCboolean alcIsExtensionPresent(ALCdevice *device, const ALCchar *extname)
{
	if (!READY)
		return 0;
	return (ALCboolean)oal_stub_d->alcIsExtensionPresent(oal_stub_h, device, extname);
}

ALCenum alcGetEnumValue(ALCdevice *device, const ALCchar *enumname)
{
	if (!READY)
		return 0;
	return oal_stub_d->alcGetEnumValue(oal_stub_h, device, enumname);
}

const ALCchar* alcGetString(ALCdevice *device, ALCenum param)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcGetString(oal_stub_h, device, param);
}

void alcGetIntegerv(ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values)
{
	if (READY)
		oal_stub_d->alcGetIntegerv(oal_stub_h, device, param, size, values);
}

ALCdevice* alcCaptureOpenDevice(const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcCaptureOpenDevice(oal_stub_h, devicename, frequency, format, buffersize);
}

ALCboolean alcCaptureCloseDevice(ALCdevice *device)
{
	if (!READY)
		return 0;
	return (ALCboolean)oal_stub_d->alcCaptureCloseDevice(oal_stub_h, device);
}

void alcCaptureStart(ALCdevice *device)
{
	if (READY)
		oal_stub_d->alcCaptureStart(oal_stub_h, device);
}

void alcCaptureStop(ALCdevice *device)
{
	if (READY)
		oal_stub_d->alcCaptureStop(oal_stub_h, device);
}

void alcCaptureSamples(ALCdevice *device, ALCvoid *buffer, ALCsizei samples)
{
	if (READY)
		oal_stub_d->alcCaptureSamples(oal_stub_h, device, buffer, samples);
}

void alEnable(ALenum capability)
{
	if (READY)
		oal_stub_d->alEnable(oal_stub_h, capability);
}

void alDisable(ALenum capability)
{
	if (READY)
		oal_stub_d->alDisable(oal_stub_h, capability);
}

ALboolean alIsEnabled(ALenum capability)
{
	if (!READY)
		return 0;
	return (ALboolean)oal_stub_d->alIsEnabled(oal_stub_h, capability);
}

const ALchar* alGetString(ALenum param)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alGetString(oal_stub_h, param);
}

void alGetBooleanv(ALenum param, ALboolean *values)
{
	if (READY)
		oal_stub_d->alGetBooleanv(oal_stub_h, param, values);
}

void alGetIntegerv(ALenum param, ALint *values)
{
	if (READY)
		oal_stub_d->alGetIntegerv(oal_stub_h, param, values);
}

void alGetFloatv(ALenum param, ALfloat *values)
{
	if (READY)
		oal_stub_d->alGetFloatv(oal_stub_h, param, values);
}

void alGetDoublev(ALenum param, ALdouble *values)
{
	if (READY)
		oal_stub_d->alGetDoublev(oal_stub_h, param, values);
}

ALboolean alGetBoolean(ALenum param)
{
	if (!READY)
		return 0;
	return (ALboolean)oal_stub_d->alGetBoolean(oal_stub_h, param);
}

ALint alGetInteger(ALenum param)
{
	if (!READY)
		return 0;
	return oal_stub_d->alGetInteger(oal_stub_h, param);
}

ALfloat alGetFloat(ALenum param)
{
	ALfloat r = 0;

	if (READY)
		oal_stub_d->alGetFloat(oal_stub_h, param, &r);
	return r;
}

ALdouble alGetDouble(ALenum param)
{
	ALdouble r = 0;

	if (READY)
		oal_stub_d->alGetDouble(oal_stub_h, param, &r);
	return r;
}

ALenum alGetError(void)
{
	if (!READY)
		return 0;
	return oal_stub_d->alGetError(oal_stub_h);
}

ALboolean alIsExtensionPresent(const ALchar *extname)
{
	if (!READY)
		return 0;
	return (ALboolean)oal_stub_d->alIsExtensionPresent(oal_stub_h, extname);
}

ALenum alGetEnumValue(const ALchar *ename)
{
	if (!READY)
		return 0;
	return oal_stub_d->alGetEnumValue(oal_stub_h, ename);
}

void alListenerf(ALenum param, ALfloat value)
{
	if (READY)
		oal_stub_d->alListenerf(oal_stub_h, param, &value);
}

void alListener3f(ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
	if (READY)
		oal_stub_d->alListener3f(oal_stub_h, param, &value1, &value2, &value3);
}

void alListenerfv(ALenum param, const ALfloat *values)
{
	if (READY)
		oal_stub_d->alListenerfv(oal_stub_h, param, values);
}

void alListeneri(ALenum param, ALint value)
{
	if (READY)
		oal_stub_d->alListeneri(oal_stub_h, param, value);
}

void alListener3i(ALenum param, ALint value1, ALint value2, ALint value3)
{
	if (READY)
		oal_stub_d->alListener3i(oal_stub_h, param, value1, value2, value3);
}

void alListeneriv(ALenum param, const ALint *values)
{
	if (READY)
		oal_stub_d->alListeneriv(oal_stub_h, param, values);
}

void alGetListenerf(ALenum param, ALfloat *value)
{
	if (READY)
		oal_stub_d->alGetListenerf(oal_stub_h, param, value);
}

void alGetListener3f(ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
	if (READY)
		oal_stub_d->alGetListener3f(oal_stub_h, param, value1, value2, value3);
}

void alGetListenerfv(ALenum param, ALfloat *values)
{
	if (READY)
		oal_stub_d->alGetListenerfv(oal_stub_h, param, values);
}

void alGetListeneri(ALenum param, ALint *value)
{
	if (READY)
		oal_stub_d->alGetListeneri(oal_stub_h, param, value);
}

void alGetListener3i(ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
	if (READY)
		oal_stub_d->alGetListener3i(oal_stub_h, param, value1, value2, value3);
}

void alGetListeneriv(ALenum param, ALint *values)
{
	if (READY)
		oal_stub_d->alGetListeneriv(oal_stub_h, param, values);
}

void alGenSources(ALsizei n, ALuint *sources)
{
	if (READY)
		oal_stub_d->alGenSources(oal_stub_h, n, sources);
}

void alDeleteSources(ALsizei n, const ALuint *sources)
{
	if (READY)
		oal_stub_d->alDeleteSources(oal_stub_h, n, sources);
}

ALboolean alIsSource(ALuint source)
{
	if (!READY)
		return 0;
	return (ALboolean)oal_stub_d->alIsSource(oal_stub_h, source);
}

void alSourcef(ALuint source, ALenum param, ALfloat value)
{
	if (READY)
		oal_stub_d->alSourcef(oal_stub_h, source, param, &value);
}

void alSource3f(ALuint source, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
	if (READY)
		oal_stub_d->alSource3f(oal_stub_h, source, param, &value1, &value2, &value3);
}

void alSourcefv(ALuint source, ALenum param, const ALfloat *values)
{
	if (READY)
		oal_stub_d->alSourcefv(oal_stub_h, source, param, values);
}

void alSourcei(ALuint source, ALenum param, ALint value)
{
	if (READY)
		oal_stub_d->alSourcei(oal_stub_h, source, param, value);
}

void alSource3i(ALuint source, ALenum param, ALint value1, ALint value2, ALint value3)
{
	if (READY)
		oal_stub_d->alSource3i(oal_stub_h, source, param, value1, value2, value3);
}

void alSourceiv(ALuint source, ALenum param, const ALint *values)
{
	if (READY)
		oal_stub_d->alSourceiv(oal_stub_h, source, param, values);
}

void alGetSourcef(ALuint source, ALenum param, ALfloat *value)
{
	if (READY)
		oal_stub_d->alGetSourcef(oal_stub_h, source, param, value);
}

void alGetSource3f(ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
	if (READY)
		oal_stub_d->alGetSource3f(oal_stub_h, source, param, value1, value2, value3);
}

void alGetSourcefv(ALuint source, ALenum param, ALfloat *values)
{
	if (READY)
		oal_stub_d->alGetSourcefv(oal_stub_h, source, param, values);
}

void alGetSourcei(ALuint source, ALenum param, ALint *value)
{
	if (READY)
		oal_stub_d->alGetSourcei(oal_stub_h, source, param, value);
}

void alGetSource3i(ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
	if (READY)
		oal_stub_d->alGetSource3i(oal_stub_h, source, param, value1, value2, value3);
}

void alGetSourceiv(ALuint source, ALenum param, ALint *values)
{
	if (READY)
		oal_stub_d->alGetSourceiv(oal_stub_h, source, param, values);
}

void alSourcePlayv(ALsizei n, const ALuint *sources)
{
	if (READY)
		oal_stub_d->alSourcePlayv(oal_stub_h, n, sources);
}

void alSourceStopv(ALsizei n, const ALuint *sources)
{
	if (READY)
		oal_stub_d->alSourceStopv(oal_stub_h, n, sources);
}

void alSourceRewindv(ALsizei n, const ALuint *sources)
{
	if (READY)
		oal_stub_d->alSourceRewindv(oal_stub_h, n, sources);
}

void alSourcePausev(ALsizei n, const ALuint *sources)
{
	if (READY)
		oal_stub_d->alSourcePausev(oal_stub_h, n, sources);
}

void alSourcePlay(ALuint source)
{
	if (READY)
		oal_stub_d->alSourcePlay(oal_stub_h, source);
}

void alSourceStop(ALuint source)
{
	if (READY)
		oal_stub_d->alSourceStop(oal_stub_h, source);
}

void alSourceRewind(ALuint source)
{
	if (READY)
		oal_stub_d->alSourceRewind(oal_stub_h, source);
}

void alSourcePause(ALuint source)
{
	if (READY)
		oal_stub_d->alSourcePause(oal_stub_h, source);
}

void alSourceQueueBuffers(ALuint source, ALsizei nb, const ALuint *buffers)
{
	if (READY)
		oal_stub_d->alSourceQueueBuffers(oal_stub_h, source, nb, buffers);
}

void alSourceUnqueueBuffers(ALuint source, ALsizei nb, ALuint *buffers)
{
	if (READY)
		oal_stub_d->alSourceUnqueueBuffers(oal_stub_h, source, nb, buffers);
}

void alGenBuffers(ALsizei n, ALuint *buffers)
{
	if (READY)
		oal_stub_d->alGenBuffers(oal_stub_h, n, buffers);
}

void alDeleteBuffers(ALsizei n, const ALuint *buffers)
{
	if (READY)
		oal_stub_d->alDeleteBuffers(oal_stub_h, n, buffers);
}

ALboolean alIsBuffer(ALuint buffer)
{
	if (!READY)
		return 0;
	return (ALboolean)oal_stub_d->alIsBuffer(oal_stub_h, buffer);
}

void alBufferData(ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq)
{
	if (READY)
		oal_stub_d->alBufferData(oal_stub_h, buffer, format, data, size, freq);
}

void alBufferf(ALuint buffer, ALenum param, ALfloat value)
{
	if (READY)
		oal_stub_d->alBufferf(oal_stub_h, buffer, param, &value);
}

void alBuffer3f(ALuint buffer, ALenum param, ALfloat value1, ALfloat value2, ALfloat value3)
{
	if (READY)
		oal_stub_d->alBuffer3f(oal_stub_h, buffer, param, &value1, &value2, &value3);
}

void alBufferfv(ALuint buffer, ALenum param, const ALfloat *values)
{
	if (READY)
		oal_stub_d->alBufferfv(oal_stub_h, buffer, param, values);
}

void alBufferi(ALuint buffer, ALenum param, ALint value)
{
	if (READY)
		oal_stub_d->alBufferi(oal_stub_h, buffer, param, value);
}

void alBuffer3i(ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3)
{
	if (READY)
		oal_stub_d->alBuffer3i(oal_stub_h, buffer, param, value1, value2, value3);
}

void alBufferiv(ALuint buffer, ALenum param, const ALint *values)
{
	if (READY)
		oal_stub_d->alBufferiv(oal_stub_h, buffer, param, values);
}

void alGetBufferf(ALuint buffer, ALenum param, ALfloat *value)
{
	if (READY)
		oal_stub_d->alGetBufferf(oal_stub_h, buffer, param, value);
}

void alGetBuffer3f(ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
	if (READY)
		oal_stub_d->alGetBuffer3f(oal_stub_h, buffer, param, value1, value2, value3);
}

void alGetBufferfv(ALuint buffer, ALenum param, ALfloat *values)
{
	if (READY)
		oal_stub_d->alGetBufferfv(oal_stub_h, buffer, param, values);
}

void alGetBufferi(ALuint buffer, ALenum param, ALint *value)
{
	if (READY)
		oal_stub_d->alGetBufferi(oal_stub_h, buffer, param, value);
}

void alGetBuffer3i(ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3)
{
	if (READY)
		oal_stub_d->alGetBuffer3i(oal_stub_h, buffer, param, value1, value2, value3);
}

void alGetBufferiv(ALuint buffer, ALenum param, ALint *values)
{
	if (READY)
		oal_stub_d->alGetBufferiv(oal_stub_h, buffer, param, values);
}

void alDopplerFactor(ALfloat value)
{
	if (READY)
		oal_stub_d->alDopplerFactor(oal_stub_h, &value);
}

void alDopplerVelocity(ALfloat value)
{
	if (READY)
		oal_stub_d->alDopplerVelocity(oal_stub_h, &value);
}

void alSpeedOfSound(ALfloat value)
{
	if (READY)
		oal_stub_d->alSpeedOfSound(oal_stub_h, &value);
}

void alDistanceModel(ALenum distanceModel)
{
	if (READY)
		oal_stub_d->alDistanceModel(oal_stub_h, distanceModel);
}

ALCdevice* alcLoopbackOpenDeviceSOFT(const ALCchar *deviceName)
{
	if (!READY)
		return NULL;
	return oal_stub_d->alcLoopbackOpenDeviceSOFT(oal_stub_h, deviceName);
}

ALCboolean alcIsRenderFormatSupportedSOFT(ALCdevice *device, ALCsizei freq, ALCenum channels, ALCenum type)
{
	if (!READY)
		return 0;
	return (ALCboolean)oal_stub_d->alcIsRenderFormatSupportedSOFT(oal_stub_h, device, freq, channels, type);
}

void alcRenderSamplesSOFT(ALCdevice *device, ALCvoid *buffer, ALCsizei samples)
{
	if (READY)
		oal_stub_d->alcRenderSamplesSOFT(oal_stub_h, device, buffer, samples);
}

/* alGetProcAddress / alcGetProcAddress: this program's own functions */
static const struct { const char *name; void *func; } procs[] = {
	{ "alcCreateContext", (void *)alcCreateContext },
	{ "alcMakeContextCurrent", (void *)alcMakeContextCurrent },
	{ "alcProcessContext", (void *)alcProcessContext },
	{ "alcSuspendContext", (void *)alcSuspendContext },
	{ "alcDestroyContext", (void *)alcDestroyContext },
	{ "alcGetCurrentContext", (void *)alcGetCurrentContext },
	{ "alcGetContextsDevice", (void *)alcGetContextsDevice },
	{ "alcOpenDevice", (void *)alcOpenDevice },
	{ "alcCloseDevice", (void *)alcCloseDevice },
	{ "alcGetError", (void *)alcGetError },
	{ "alcIsExtensionPresent", (void *)alcIsExtensionPresent },
	{ "alcGetProcAddress", (void *)alcGetProcAddress },
	{ "alcGetEnumValue", (void *)alcGetEnumValue },
	{ "alcGetString", (void *)alcGetString },
	{ "alcGetIntegerv", (void *)alcGetIntegerv },
	{ "alcCaptureOpenDevice", (void *)alcCaptureOpenDevice },
	{ "alcCaptureCloseDevice", (void *)alcCaptureCloseDevice },
	{ "alcCaptureStart", (void *)alcCaptureStart },
	{ "alcCaptureStop", (void *)alcCaptureStop },
	{ "alcCaptureSamples", (void *)alcCaptureSamples },
	{ "alEnable", (void *)alEnable },
	{ "alDisable", (void *)alDisable },
	{ "alIsEnabled", (void *)alIsEnabled },
	{ "alGetString", (void *)alGetString },
	{ "alGetBooleanv", (void *)alGetBooleanv },
	{ "alGetIntegerv", (void *)alGetIntegerv },
	{ "alGetFloatv", (void *)alGetFloatv },
	{ "alGetDoublev", (void *)alGetDoublev },
	{ "alGetBoolean", (void *)alGetBoolean },
	{ "alGetInteger", (void *)alGetInteger },
	{ "alGetFloat", (void *)alGetFloat },
	{ "alGetDouble", (void *)alGetDouble },
	{ "alGetError", (void *)alGetError },
	{ "alIsExtensionPresent", (void *)alIsExtensionPresent },
	{ "alGetProcAddress", (void *)alGetProcAddress },
	{ "alGetEnumValue", (void *)alGetEnumValue },
	{ "alListenerf", (void *)alListenerf },
	{ "alListener3f", (void *)alListener3f },
	{ "alListenerfv", (void *)alListenerfv },
	{ "alListeneri", (void *)alListeneri },
	{ "alListener3i", (void *)alListener3i },
	{ "alListeneriv", (void *)alListeneriv },
	{ "alGetListenerf", (void *)alGetListenerf },
	{ "alGetListener3f", (void *)alGetListener3f },
	{ "alGetListenerfv", (void *)alGetListenerfv },
	{ "alGetListeneri", (void *)alGetListeneri },
	{ "alGetListener3i", (void *)alGetListener3i },
	{ "alGetListeneriv", (void *)alGetListeneriv },
	{ "alGenSources", (void *)alGenSources },
	{ "alDeleteSources", (void *)alDeleteSources },
	{ "alIsSource", (void *)alIsSource },
	{ "alSourcef", (void *)alSourcef },
	{ "alSource3f", (void *)alSource3f },
	{ "alSourcefv", (void *)alSourcefv },
	{ "alSourcei", (void *)alSourcei },
	{ "alSource3i", (void *)alSource3i },
	{ "alSourceiv", (void *)alSourceiv },
	{ "alGetSourcef", (void *)alGetSourcef },
	{ "alGetSource3f", (void *)alGetSource3f },
	{ "alGetSourcefv", (void *)alGetSourcefv },
	{ "alGetSourcei", (void *)alGetSourcei },
	{ "alGetSource3i", (void *)alGetSource3i },
	{ "alGetSourceiv", (void *)alGetSourceiv },
	{ "alSourcePlayv", (void *)alSourcePlayv },
	{ "alSourceStopv", (void *)alSourceStopv },
	{ "alSourceRewindv", (void *)alSourceRewindv },
	{ "alSourcePausev", (void *)alSourcePausev },
	{ "alSourcePlay", (void *)alSourcePlay },
	{ "alSourceStop", (void *)alSourceStop },
	{ "alSourceRewind", (void *)alSourceRewind },
	{ "alSourcePause", (void *)alSourcePause },
	{ "alSourceQueueBuffers", (void *)alSourceQueueBuffers },
	{ "alSourceUnqueueBuffers", (void *)alSourceUnqueueBuffers },
	{ "alGenBuffers", (void *)alGenBuffers },
	{ "alDeleteBuffers", (void *)alDeleteBuffers },
	{ "alIsBuffer", (void *)alIsBuffer },
	{ "alBufferData", (void *)alBufferData },
	{ "alBufferf", (void *)alBufferf },
	{ "alBuffer3f", (void *)alBuffer3f },
	{ "alBufferfv", (void *)alBufferfv },
	{ "alBufferi", (void *)alBufferi },
	{ "alBuffer3i", (void *)alBuffer3i },
	{ "alBufferiv", (void *)alBufferiv },
	{ "alGetBufferf", (void *)alGetBufferf },
	{ "alGetBuffer3f", (void *)alGetBuffer3f },
	{ "alGetBufferfv", (void *)alGetBufferfv },
	{ "alGetBufferi", (void *)alGetBufferi },
	{ "alGetBuffer3i", (void *)alGetBuffer3i },
	{ "alGetBufferiv", (void *)alGetBufferiv },
	{ "alDopplerFactor", (void *)alDopplerFactor },
	{ "alDopplerVelocity", (void *)alDopplerVelocity },
	{ "alSpeedOfSound", (void *)alSpeedOfSound },
	{ "alDistanceModel", (void *)alDistanceModel },
	{ "alcLoopbackOpenDeviceSOFT", (void *)alcLoopbackOpenDeviceSOFT },
	{ "alcIsRenderFormatSupportedSOFT", (void *)alcIsRenderFormatSupportedSOFT },
	{ "alcRenderSamplesSOFT", (void *)alcRenderSamplesSOFT },
};

static void *find_proc(const char *fname)
{
	unsigned i;

	if (!fname)
		return NULL;
	for (i = 0; i < sizeof(procs) / sizeof(procs[0]); i++) {
		if (strcmp(procs[i].name, fname) == 0)
			return procs[i].func;
	}
	return NULL;
}

void *alGetProcAddress(const ALchar *fname)
{
	return find_proc(fname);
}

void *alcGetProcAddress(ALCdevice *device, const ALCchar *funcname)
{
	return find_proc(funcname);
}
