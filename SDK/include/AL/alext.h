/*
 * The OpenAL extensions openal-amiga implements.
 *
 * Only what the library supports is declared here. Programs ask for an
 * extension with alIsExtensionPresent / alcIsExtensionPresent and fall
 * back when it is missing, so a missing extension is not an error.
 */

#ifndef AL_ALEXT_H
#define AL_ALEXT_H

#include "al.h"
#include "alc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* AL_EXT_FLOAT32: 32-bit float sample formats */
#ifndef AL_EXT_float32
#define AL_EXT_float32 1
#define AL_FORMAT_MONO_FLOAT32                   0x10010
#define AL_FORMAT_STEREO_FLOAT32                 0x10011
#endif

/* AL_EXT_LINEAR_DISTANCE, AL_EXT_EXPONENT_DISTANCE and AL_EXT_OFFSET are
   part of OpenAL 1.1; their enums are in al.h. */

/* ALC_SOFT_loopback: render the mix into a buffer the program supplies
   instead of playing it. The test programs use it to check the mixer
   without sound hardware. Only stereo 16-bit output is supported. */
#ifndef ALC_SOFT_loopback
#define ALC_SOFT_loopback 1
#define ALC_FORMAT_CHANNELS_SOFT                 0x1990
#define ALC_FORMAT_TYPE_SOFT                     0x1991

#define ALC_BYTE_SOFT                            0x1400
#define ALC_UNSIGNED_BYTE_SOFT                   0x1401
#define ALC_SHORT_SOFT                           0x1402
#define ALC_UNSIGNED_SHORT_SOFT                  0x1403
#define ALC_INT_SOFT                             0x1404
#define ALC_UNSIGNED_INT_SOFT                    0x1405
#define ALC_FLOAT_SOFT                           0x1406

#define ALC_MONO_SOFT                            0x1500
#define ALC_STEREO_SOFT                          0x1501
#define ALC_QUAD_SOFT                            0x1503
#define ALC_5POINT1_SOFT                         0x1504
#define ALC_6POINT1_SOFT                         0x1505
#define ALC_7POINT1_SOFT                         0x1506

typedef ALCdevice* (ALC_APIENTRY *LPALCLOOPBACKOPENDEVICESOFT)(const ALCchar *deviceName);
typedef ALCboolean (ALC_APIENTRY *LPALCISRENDERFORMATSUPPORTEDSOFT)(ALCdevice *device, ALCsizei freq, ALCenum channels, ALCenum type);
typedef void (ALC_APIENTRY *LPALCRENDERSAMPLESSOFT)(ALCdevice *device, ALCvoid *buffer, ALCsizei samples);

ALC_API ALCdevice* ALC_APIENTRY alcLoopbackOpenDeviceSOFT(const ALCchar *deviceName);
ALC_API ALCboolean ALC_APIENTRY alcIsRenderFormatSupportedSOFT(ALCdevice *device, ALCsizei freq, ALCenum channels, ALCenum type);
ALC_API void ALC_APIENTRY alcRenderSamplesSOFT(ALCdevice *device, ALCvoid *buffer, ALCsizei samples);
#endif

#ifdef __cplusplus
}
#endif

#endif /* AL_ALEXT_H */
