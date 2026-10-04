# openal.library developer guide

openal.library provides OpenAL 1.1 on AmigaOS 3.x (68k). Programs written
for OpenAL build against the headers in `SDK/include` without changes.
This guide covers how to link against it, how the library is called,
and what to know when porting a game.

## Requirements

- AmigaOS 3.x
- A 68040 or 68060 with FPU, or an Emu68 machine (PiStorm). The library
  is built with `-m68040`.
- AHI, with a mode set for the unit used (unit 0 unless
  `ENV:OpenAL/Unit` says otherwise)

## Contents of the SDK

```
SDK/include/AL/           al.h, alc.h, alext.h: the OpenAL API
SDK/include/libraries/    openal_dispatch.h: the library's function table
SDK/include/proto/        openal.h: picks the right file for your compiler
SDK/include/clib/         openal_protos.h
SDK/include/inline/       openal.h (gcc), openal_protos.h (vbcc)
SDK/include/pragmas/      openal_pragmas.h (SAS/C)
SDK/fd, SDK/sfd           openal_lib.fd, openal_lib.sfd
SDK/gcc/libopenalshared.a stubs for openal.library, gcc
SDK/gcc/libopenal.a       the whole of OpenAL as a static library, gcc
SDK/vbcc/openal.lib       stubs for openal.library, vbcc
SDK/sasc/                 stub sources and smakefile for SAS/C
```

## Linking

You write ordinary OpenAL code and choose the library at link time.

**gcc (bebbo's m68k-amigaos-gcc), with openal.library:**

```
m68k-amigaos-gcc -noixemul -m68040 -ISDK/include -c game.c
m68k-amigaos-gcc -noixemul -m68040 game.o -o game -LSDK/gcc -lopenalshared
```

**gcc, static** (OpenAL becomes part of the program and openal.library
is not needed):

```
m68k-amigaos-gcc -noixemul -m68040 game.o -o game -LSDK/gcc -lopenal
```

**vbcc:**

```
vc -c99 -cpu=68040 -fpu=68040 -ISDK/include game.c -o game SDK/vbcc/openal.lib -lamiga
```

**SAS/C 6.x:** build `openal.lib` with the `smakefile` in `SDK/sasc`
first, then:

```
sc LINK game.c IDIR=SDK/include LIB SDK/sasc/openal.lib
```

The SAS/C files have not been compiled yet: no SAS/C was at hand. The
gcc and vbcc libraries are tested.

## How a program uses the library

The stubs (`libopenalshared.a`, `openal.lib`) contain every OpenAL
function. The first call opens `openal.library`, and `atexit` closes it
again. If the library cannot be opened, every call does nothing and
`alcOpenDevice` returns NULL. A game then runs on without sound instead
of failing.

When the program exits, the library closes anything the program left
open: its devices, the mixer task and AHI.

Several programs can use the library at the same time. Each has its own
current context and error state. All programs play through one mixer
task and one AHI stream, so they are heard together even when the AHI
unit has only one channel.

## How the library is called

Programs normally do not need this. It matters if you write your own
stubs, or bindings for another language.

Besides the four standard functions, the library has one entry point:

```
const struct OALDispatch *OALGetDispatch(ULONG abi)   /* d0, LVO -30 */
```

It returns a table of C function pointers (`SDK/include/libraries/
openal_dispatch.h`), or NULL if the library does not serve that ABI
version. Call it with `OAL_ABI_VERSION`. Then:

1. Call `oal_create_handle()` once. Pass the handle as the first argument
   of every function in the table.
2. Call `oal_destroy_handle(handle)` before `CloseLibrary()`.

So that code from any compiler can call the table, only 32-bit integers
and pointers cross it:

- **Float and double parameters** are passed by pointer: compilers do not
  agree on how a float goes on the stack.
- **Float and double results** come back through a pointer passed as the
  last argument: depending on the compiler they would be in `fp0` or
  `d0`.
- **ALboolean results** are returned as ALint.
- **All arguments go on the stack.** `OAL_CALL` adds `__stdargs` for
  SAS/C.

`oal_count` tells how many functions the library has. Functions are only
ever added at the end of the table, so a program works with any library
whose `oal_count` is at least the `OAL_FUNC_COUNT` it was built with.

## Porting notes

- **Engines that load OpenAL at run time:** ioquake3-based games load it
  with `dlopen` / `Sys_LoadDll` (`qal.c`). Change them to call the
  functions directly, or to get them with `alGetProcAddress` /
  `alcGetProcAddress`. With the stubs these return the stub functions and
  work before a context exists.
- **Device names:** `alcOpenDevice(NULL)`, `"AHI"` and the usual Windows
  device names all open the AHI output.
- **16-bit sample data** is in the machine's byte order (big-endian), as
  OpenAL specifies. WAV files are little-endian: swap their bytes before
  `alBufferData` (see `test/src/common.h`).
- **No console output while MiniGL holds the display:** the library
  never writes to the console; its debug log goes to `T:openal.log`.
  Writing to the console while `minigl.library` holds the display can lock
  up the machine.
- **Tasks:** OpenAL may be called from any task. The library allocates
  memory with `AllocVec`.
- **Structure alignment:** the API has no structures, so it works with
  programs built with or without `-malign-int`.

### gcc 6.5 and -m68040

bebbo's m68k-amigaos-gcc 6.5 miscompiles some FPU code with `-m68040`.
When an instruction that rounds to single precision reads a double
through `(aN)+`, gcc assumes the register moved by 4 bytes, while the
CPU moves it by 8. Every later offset from that register is then wrong
by 4 bytes. In openal-amiga this turned the y and z of `alSource3f` into
garbage. Look for this pattern in the `gcc -S` output of your own code:

```
fsmove.d (a3)+,fp0      reads v[0]; a3 += 8
fsmove.d (4,a3),fp0     meant v[1], which is at (0,a3)
```

The library and the SDK are checked for it on every build.

## Supported features

- OpenAL 1.1 complete: sources, buffers, listener, all distance models,
  cones, doppler, offsets, streaming with buffer queues
- Formats: mono and stereo, 8 bit, 16 bit, 32-bit float
  (`AL_EXT_FLOAT32`)
- Extensions: `AL_EXT_FLOAT32`, `AL_EXT_OFFSET`,
  `AL_EXT_LINEAR_DISTANCE`, `AL_EXT_EXPONENT_DISTANCE`,
  `ALC_ENUMERATION_EXT`, `ALC_ENUMERATE_ALL_EXT`, `ALC_SOFT_loopback`
- Stereo output; the panning follows OpenAL Soft's default stereo
  output, measured from it
- Not supported: capture, EFX, HRTF, more than two output channels

## Settings

Environment variables in `ENV:OpenAL/` (`MakeDir ENV:OpenAL` first):

| Variable | Default | Meaning |
|---|---|---|
| `Frequency` | 22050 | Mixing rate in Hz |
| `Period` | 1024 | Frames per output buffer; the latency is about two periods |
| `Unit` | 0 | AHI unit, 0-3 |
| `Priority` | 5 | Priority of the mixer process |
| `Debug` | 0 | 1: write a log to `T:openal.log` |
