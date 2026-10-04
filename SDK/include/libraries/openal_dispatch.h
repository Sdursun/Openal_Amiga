/* Generated from library/functions.list by gen_dispatch.awk; do not edit. */

#ifndef LIBRARIES_OPENAL_DISPATCH_H
#define LIBRARIES_OPENAL_DISPATCH_H

#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alext.h>

/* Raised only if the table's layout changes in a way old programs cannot
   follow. Appending functions does not change it: oal_count tells how
   many the library has. */
#define OAL_ABI_VERSION 2
#define OAL_FUNC_COUNT 96

/* The library's only entry point after the standard four:
   struct OALDispatch *OALGetDispatch(ULONG abi_version) (d0), LVO -30
   (see <proto/openal.h>). Returns NULL if the library does not serve
   that ABI version. */
#define OAL_LVO_GETDISPATCH (-30)

/* Only 32-bit integers and pointers cross this table, so code from any
   compiler can call it: float parameters by pointer, float results
   through a result pointer, boolean results widened to ALint.
   Arguments always go on the stack: OAL_CALL makes sure of that
   for SAS/C code built with PARMS=REGISTER. */
#if defined(__SASC)
#define OAL_CALL __stdargs
#else
#define OAL_CALL
#endif

struct OALDispatch {
	unsigned long oal_abi_version;
	unsigned long oal_struct_size;
	unsigned long oal_count;          /* functions after the two below */
	unsigned long oal_reserved;
	void *(OAL_CALL *oal_create_handle)(void);
	void (OAL_CALL *oal_destroy_handle)(void *h);
	ALCcontext* (OAL_CALL *alcCreateContext)(void *h, ALCdevice *device, const ALCint *attrlist);
	ALint (OAL_CALL *alcMakeContextCurrent)(void *h, ALCcontext *context);
	void (OAL_CALL *alcProcessContext)(void *h, ALCcontext *context);
	void (OAL_CALL *alcSuspendContext)(void *h, ALCcontext *context);
	void (OAL_CALL *alcDestroyContext)(void *h, ALCcontext *context);
	ALCcontext* (OAL_CALL *alcGetCurrentContext)(void *h);
	ALCdevice* (OAL_CALL *alcGetContextsDevice)(void *h, ALCcontext *context);
	ALCdevice* (OAL_CALL *alcOpenDevice)(void *h, const ALCchar *devicename);
	ALint (OAL_CALL *alcCloseDevice)(void *h, ALCdevice *device);
	ALCenum (OAL_CALL *alcGetError)(void *h, ALCdevice *device);
	ALint (OAL_CALL *alcIsExtensionPresent)(void *h, ALCdevice *device, const ALCchar *extname);
	void* (OAL_CALL *alcGetProcAddress)(void *h, ALCdevice *device, const ALCchar *funcname);
	ALCenum (OAL_CALL *alcGetEnumValue)(void *h, ALCdevice *device, const ALCchar *enumname);
	const ALCchar* (OAL_CALL *alcGetString)(void *h, ALCdevice *device, ALCenum param);
	void (OAL_CALL *alcGetIntegerv)(void *h, ALCdevice *device, ALCenum param, ALCsizei size, ALCint *values);
	ALCdevice* (OAL_CALL *alcCaptureOpenDevice)(void *h, const ALCchar *devicename, ALCuint frequency, ALCenum format, ALCsizei buffersize);
	ALint (OAL_CALL *alcCaptureCloseDevice)(void *h, ALCdevice *device);
	void (OAL_CALL *alcCaptureStart)(void *h, ALCdevice *device);
	void (OAL_CALL *alcCaptureStop)(void *h, ALCdevice *device);
	void (OAL_CALL *alcCaptureSamples)(void *h, ALCdevice *device, ALCvoid *buffer, ALCsizei samples);
	void (OAL_CALL *alEnable)(void *h, ALenum capability);
	void (OAL_CALL *alDisable)(void *h, ALenum capability);
	ALint (OAL_CALL *alIsEnabled)(void *h, ALenum capability);
	const ALchar* (OAL_CALL *alGetString)(void *h, ALenum param);
	void (OAL_CALL *alGetBooleanv)(void *h, ALenum param, ALboolean *values);
	void (OAL_CALL *alGetIntegerv)(void *h, ALenum param, ALint *values);
	void (OAL_CALL *alGetFloatv)(void *h, ALenum param, ALfloat *values);
	void (OAL_CALL *alGetDoublev)(void *h, ALenum param, ALdouble *values);
	ALint (OAL_CALL *alGetBoolean)(void *h, ALenum param);
	ALint (OAL_CALL *alGetInteger)(void *h, ALenum param);
	void (OAL_CALL *alGetFloat)(void *h, ALenum param, ALfloat *result);
	void (OAL_CALL *alGetDouble)(void *h, ALenum param, ALdouble *result);
	ALenum (OAL_CALL *alGetError)(void *h);
	ALint (OAL_CALL *alIsExtensionPresent)(void *h, const ALchar *extname);
	void* (OAL_CALL *alGetProcAddress)(void *h, const ALchar *fname);
	ALenum (OAL_CALL *alGetEnumValue)(void *h, const ALchar *ename);
	void (OAL_CALL *alListenerf)(void *h, ALenum param, const ALfloat *value);
	void (OAL_CALL *alListener3f)(void *h, ALenum param, const ALfloat *value1, const ALfloat *value2, const ALfloat *value3);
	void (OAL_CALL *alListenerfv)(void *h, ALenum param, const ALfloat *values);
	void (OAL_CALL *alListeneri)(void *h, ALenum param, ALint value);
	void (OAL_CALL *alListener3i)(void *h, ALenum param, ALint value1, ALint value2, ALint value3);
	void (OAL_CALL *alListeneriv)(void *h, ALenum param, const ALint *values);
	void (OAL_CALL *alGetListenerf)(void *h, ALenum param, ALfloat *value);
	void (OAL_CALL *alGetListener3f)(void *h, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
	void (OAL_CALL *alGetListenerfv)(void *h, ALenum param, ALfloat *values);
	void (OAL_CALL *alGetListeneri)(void *h, ALenum param, ALint *value);
	void (OAL_CALL *alGetListener3i)(void *h, ALenum param, ALint *value1, ALint *value2, ALint *value3);
	void (OAL_CALL *alGetListeneriv)(void *h, ALenum param, ALint *values);
	void (OAL_CALL *alGenSources)(void *h, ALsizei n, ALuint *sources);
	void (OAL_CALL *alDeleteSources)(void *h, ALsizei n, const ALuint *sources);
	ALint (OAL_CALL *alIsSource)(void *h, ALuint source);
	void (OAL_CALL *alSourcef)(void *h, ALuint source, ALenum param, const ALfloat *value);
	void (OAL_CALL *alSource3f)(void *h, ALuint source, ALenum param, const ALfloat *value1, const ALfloat *value2, const ALfloat *value3);
	void (OAL_CALL *alSourcefv)(void *h, ALuint source, ALenum param, const ALfloat *values);
	void (OAL_CALL *alSourcei)(void *h, ALuint source, ALenum param, ALint value);
	void (OAL_CALL *alSource3i)(void *h, ALuint source, ALenum param, ALint value1, ALint value2, ALint value3);
	void (OAL_CALL *alSourceiv)(void *h, ALuint source, ALenum param, const ALint *values);
	void (OAL_CALL *alGetSourcef)(void *h, ALuint source, ALenum param, ALfloat *value);
	void (OAL_CALL *alGetSource3f)(void *h, ALuint source, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
	void (OAL_CALL *alGetSourcefv)(void *h, ALuint source, ALenum param, ALfloat *values);
	void (OAL_CALL *alGetSourcei)(void *h, ALuint source, ALenum param, ALint *value);
	void (OAL_CALL *alGetSource3i)(void *h, ALuint source, ALenum param, ALint *value1, ALint *value2, ALint *value3);
	void (OAL_CALL *alGetSourceiv)(void *h, ALuint source, ALenum param, ALint *values);
	void (OAL_CALL *alSourcePlayv)(void *h, ALsizei n, const ALuint *sources);
	void (OAL_CALL *alSourceStopv)(void *h, ALsizei n, const ALuint *sources);
	void (OAL_CALL *alSourceRewindv)(void *h, ALsizei n, const ALuint *sources);
	void (OAL_CALL *alSourcePausev)(void *h, ALsizei n, const ALuint *sources);
	void (OAL_CALL *alSourcePlay)(void *h, ALuint source);
	void (OAL_CALL *alSourceStop)(void *h, ALuint source);
	void (OAL_CALL *alSourceRewind)(void *h, ALuint source);
	void (OAL_CALL *alSourcePause)(void *h, ALuint source);
	void (OAL_CALL *alSourceQueueBuffers)(void *h, ALuint source, ALsizei nb, const ALuint *buffers);
	void (OAL_CALL *alSourceUnqueueBuffers)(void *h, ALuint source, ALsizei nb, ALuint *buffers);
	void (OAL_CALL *alGenBuffers)(void *h, ALsizei n, ALuint *buffers);
	void (OAL_CALL *alDeleteBuffers)(void *h, ALsizei n, const ALuint *buffers);
	ALint (OAL_CALL *alIsBuffer)(void *h, ALuint buffer);
	void (OAL_CALL *alBufferData)(void *h, ALuint buffer, ALenum format, const ALvoid *data, ALsizei size, ALsizei freq);
	void (OAL_CALL *alBufferf)(void *h, ALuint buffer, ALenum param, const ALfloat *value);
	void (OAL_CALL *alBuffer3f)(void *h, ALuint buffer, ALenum param, const ALfloat *value1, const ALfloat *value2, const ALfloat *value3);
	void (OAL_CALL *alBufferfv)(void *h, ALuint buffer, ALenum param, const ALfloat *values);
	void (OAL_CALL *alBufferi)(void *h, ALuint buffer, ALenum param, ALint value);
	void (OAL_CALL *alBuffer3i)(void *h, ALuint buffer, ALenum param, ALint value1, ALint value2, ALint value3);
	void (OAL_CALL *alBufferiv)(void *h, ALuint buffer, ALenum param, const ALint *values);
	void (OAL_CALL *alGetBufferf)(void *h, ALuint buffer, ALenum param, ALfloat *value);
	void (OAL_CALL *alGetBuffer3f)(void *h, ALuint buffer, ALenum param, ALfloat *value1, ALfloat *value2, ALfloat *value3);
	void (OAL_CALL *alGetBufferfv)(void *h, ALuint buffer, ALenum param, ALfloat *values);
	void (OAL_CALL *alGetBufferi)(void *h, ALuint buffer, ALenum param, ALint *value);
	void (OAL_CALL *alGetBuffer3i)(void *h, ALuint buffer, ALenum param, ALint *value1, ALint *value2, ALint *value3);
	void (OAL_CALL *alGetBufferiv)(void *h, ALuint buffer, ALenum param, ALint *values);
	void (OAL_CALL *alDopplerFactor)(void *h, const ALfloat *value);
	void (OAL_CALL *alDopplerVelocity)(void *h, const ALfloat *value);
	void (OAL_CALL *alSpeedOfSound)(void *h, const ALfloat *value);
	void (OAL_CALL *alDistanceModel)(void *h, ALenum distanceModel);
	ALCdevice* (OAL_CALL *alcLoopbackOpenDeviceSOFT)(void *h, const ALCchar *deviceName);
	ALint (OAL_CALL *alcIsRenderFormatSupportedSOFT)(void *h, ALCdevice *device, ALCsizei freq, ALCenum channels, ALCenum type);
	void (OAL_CALL *alcRenderSamplesSOFT)(void *h, ALCdevice *device, ALCvoid *buffer, ALCsizei samples);
};

#endif
