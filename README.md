# openal.library for AmigaOS 3.x

OpenAL 1.1 for 68k AmigaOS, playing through AHI. Made for porting PC
games that use OpenAL to PiStorm / Emu68 machines; first in line is
OpenMoHAA (Medal of Honor: Allied Assault).

This repository holds the library, the SDK for developers (gcc, vbcc,
SAS/C), documentation and test programs with their sources. The source
code of the library itself is not published yet.

```
libs/openal.library   the library: copy it to LIBS:
SDK/                  headers and link libraries for gcc, vbcc and SAS/C
doc/                  developer guide (English and Turkish)
test/                 test programs, sample.wav, and their sources in test/src
```

## Requirements

- AmigaOS 3.x
- A 68040 or 68060 with FPU, or Emu68 (PiStorm)
- AHI, with a mode set for unit 0 in AHI prefs

## Installing

```
Copy libs/openal.library LIBS:
```

If an older openal.library is still in memory, run `Avail FLUSH` or
reboot.

## Features

- The complete OpenAL 1.1 API: sources, buffers, listener, all distance
  models, cones, doppler, offsets, streaming with buffer queues
- Mono and stereo, 8 bit, 16 bit, 32-bit float
- Stereo panning measured from OpenAL Soft, so games sound as balanced as
  on the PC
- Several programs can use it at once. They play through one mixer and
  one AHI stream, so they are heard together even when the AHI unit has
  only one channel.
- What a program leaves open when it exits is closed by the library
- Not supported: recording, EFX effects, HRTF

## Testing

Copy the `test` directory to the Amiga and run from a Shell:

| Program | What should happen |
|---|---|
| `al_mathcheck` | No line says `WRONG` |
| `al_libtest` | `27 checks, 0 failed` |
| `al_selftest` | `157 checks, 0 failed` |
| `al_info` | The AHI device opens and its settings are listed |
| `al_tone3d` | A tone circles around you, from the front to the left, behind, to the right |
| `al_playwav sample.wav` | `sample.wav` plays; also try `LOOP` and `STREAM` |
| `al_bench SOURCES 32` | Shows how much CPU mixing 32 sources takes |

Run `al_tone3d` and `al_playwav sample.wav LOOP` in two Shells at once:
both are heard. Ctrl-C stops a program.

If something goes wrong, turn on the log and send `T:openal.log`:

```
MakeDir ENV:OpenAL
SetEnv OpenAL/Debug 1
```

### Test results

| Where | What |
|---|---|
| PiStorm, Raspberry Pi 4, Emu68 | Self test 157/157 statically and through the library; library test 26/26; AHI output, WAV playback, 3D panning; two programs at once. 32 moving 3D sources take 1.8% of the CPU. |
| vamos emulator, 68040 code | Self test 157/157 statically, through the library, and built with vbcc against the gcc-built library; library test 27/27 |
| PC | The same checks against OpenAL Soft 1.23 pass as well, so the library behaves the way games expect |

This release changed how the library's table passes floats (by pointer,
so that every compiler can call it). The hardware tests above were made
with the build before that change; this build has been tested in vamos.

## For developers

Programs use OpenAL as on any other system and pick the library when
linking:

```
gcc   -ISDK/include ... -LSDK/gcc -lopenalshared    (uses openal.library)
gcc   -ISDK/include ... -LSDK/gcc -lopenal          (static, no library needed)
vbcc  -ISDK/include ... SDK/vbcc/openal.lib -lamiga
SAS/C build SDK/sasc/openal.lib with its smakefile, then link it
```

If `openal.library` is missing, OpenAL calls do nothing and
`alcOpenDevice` returns NULL, so a game runs on without sound.

The SAS/C stubs have not been compiled yet. Everything else is tested.
See [doc/developer-guide.md](doc/developer-guide.md) for the details:
how the library is called, porting notes, settings, and a gcc 6.5
`-m68040` bug to watch for.

## Settings

Environment variables in `ENV:OpenAL/`:

| Variable | Default | Meaning |
|---|---|---|
| `Frequency` | 22050 | Mixing rate in Hz |
| `Period` | 1024 | Frames per output buffer; raise it (e.g. 2048) if the sound breaks up |
| `Unit` | 0 | AHI unit, 0-3 |
| `Priority` | 5 | Priority of the mixer process |
| `Debug` | 0 | 1: write a log to `T:openal.log` |

## About this project

The code of this library was written with
[Claude Code](https://claude.com/claude-code), Anthropic's AI coding
assistant, working together with me on real hardware: I ran the builds
on my Amiga and sent back the results, and Claude Code wrote the code,
found the problems and fixed them. As with my RTCW port, this is an
experiment in how far this way of working can go.

## License

zlib license, see [LICENSE](LICENSE).
