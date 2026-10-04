# openal.library for AmigaOS 3.x

OpenAL 1.1 for 68k AmigaOS, playing through AHI. Made for porting PC
games that use OpenAL to PiStorm / Emu68 machines; first in line is
OpenMoHAA (Medal of Honor: Allied Assault).

**Türkçe açıklama aşağıda / Turkish description below.**

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

---

## Türkçe

AmigaOS 3.x (68k) için OpenAL 1.1. Ses AHI üzerinden çalar. OpenAL
kullanan PC oyunlarını PiStorm / Emu68 makinelerine portlamak için
yapıldı; ilk hedef OpenMoHAA (Medal of Honor: Allied Assault).

Bu depoda kütüphane, geliştiriciler için SDK (gcc, vbcc, SAS/C), belgeler
ve kaynaklarıyla birlikte test programları bulunuyor. Kütüphanenin kendi
kaynak kodu henüz yayınlanmadı.

```
libs/openal.library   kütüphane: LIBS: içine kopyalayın
SDK/                  gcc, vbcc ve SAS/C için başlıklar ve bağlama kütüphaneleri
doc/                  geliştirici rehberi (Türkçe ve İngilizce)
test/                 test programları, sample.wav ve test/src içinde kaynakları
```

### Gereksinimler

- AmigaOS 3.x
- FPU'lu 68040 veya 68060, ya da Emu68 (PiStorm)
- AHI ve AHI Prefs'te birim 0 için seçilmiş bir mod

### Kurulum

```
Copy libs/openal.library LIBS:
```

Eski bir openal.library bellekte duruyorsa `Avail FLUSH` komutunu verin
veya makineyi yeniden başlatın.

### Özellikler

- OpenAL 1.1'in tamamı: kaynaklar, buffer'lar, dinleyici, tüm mesafe
  modelleri, koni, doppler, offset'ler, buffer kuyruğuyla akış
- Mono ve stereo; 8 bit, 16 bit ve 32 bit float formatlar
- openal-soft'tan ölçülen stereo panning; oyunların ses dengesi PC'deki
  gibi
- Birden fazla program aynı anda kullanabilir. Hepsi tek mikser ve tek
  AHI akışı üzerinden çalar; AHI birimi tek kanallı olsa da birlikte
  duyulurlar.
- Program çıkarken açık bıraktığı her şeyi kütüphane kapatır
- Desteklenmeyenler: kayıt, EFX efektleri, HRTF

### Test

`test` klasörünü Amiga'ya kopyalayın ve Shell'den çalıştırın:

| Program | Beklenen |
|---|---|
| `al_mathcheck` | Hiçbir satırda `WRONG` yok |
| `al_libtest` | `27 checks, 0 failed` |
| `al_selftest` | `157 checks, 0 failed` |
| `al_info` | AHI cihazı açılır ve ayarları listelenir |
| `al_tone3d` | Ses etrafınızda döner: önden sola, arkaya, sağa |
| `al_playwav sample.wav` | `sample.wav` çalar; `LOOP` ve `STREAM` ile de deneyin |
| `al_bench SOURCES 32` | 32 kaynağı mikslemenin ne kadar CPU harcadığını gösterir |

`al_tone3d` ve `al_playwav sample.wav LOOP` komutlarını iki Shell'de
aynı anda çalıştırın; ikisi birlikte duyulmalı. Ctrl-C programı durdurur.

Sorun olursa logu açıp `T:openal.log` dosyasını gönderin:

```
MakeDir ENV:OpenAL
SetEnv OpenAL/Debug 1
```

#### Test sonuçları

| Nerede | Ne |
|---|---|
| PiStorm, Raspberry Pi 4, Emu68 | Selftest statik ve kütüphane üzerinden 157/157; kütüphane testi 26/26; AHI çıkışı, WAV çalma, 3D panning; iki program aynı anda. 32 hareketli 3D kaynak CPU'nun %1,8'ini alıyor. |
| vamos emülatörü, 68040 kodu | Selftest statik, kütüphane üzerinden ve vbcc ile derlenip gcc kütüphanesine karşı 157/157; kütüphane testi 27/27 |
| PC | Aynı kontroller openal-soft 1.23'e karşı da geçiyor; kütüphane oyunların beklediği gibi davranıyor |

Bu sürümde kütüphane tablosunun float'ları geçirme şekli değişti
(her derleyicinin çağırabilmesi için işaretçiyle). Yukarıdaki donanım
testleri bu değişiklikten önceki derlemeyle yapıldı; bu derleme vamos'ta
test edildi.

### Geliştiriciler için

Programlar OpenAL'ı her sistemdeki gibi kullanır; hangi kütüphanenin
kullanılacağı bağlama sırasında seçilir:

```
gcc   -ISDK/include ... -LSDK/gcc -lopenalshared    (openal.library kullanır)
gcc   -ISDK/include ... -LSDK/gcc -lopenal          (statik, kütüphane gerekmez)
vbcc  -ISDK/include ... SDK/vbcc/openal.lib -lamiga
SAS/C SDK/sasc/openal.lib'i smakefile ile derleyip bağlayın
```

`openal.library` yoksa OpenAL çağrıları hiçbir şey yapmaz ve
`alcOpenDevice` NULL döner; oyun sessiz devam eder.

SAS/C stub'ları henüz derlenmedi; geri kalan her şey test edildi.
Ayrıntılar için [doc/gelistirici-rehberi.md](doc/gelistirici-rehberi.md):
kütüphanenin nasıl çağrıldığı, port notları, ayarlar ve gcc 6.5'teki
bir `-m68040` hatası.

### Ayarlar

`ENV:OpenAL/` içindeki ortam değişkenleri:

| Değişken | Varsayılan | Anlamı |
|---|---|---|
| `Frequency` | 22050 | Miksleme hızı (Hz) |
| `Period` | 1024 | Çıkış buffer'ı başına frame; ses kesiliyorsa artırın (ör. 2048) |
| `Unit` | 0 | AHI birimi, 0-3 |
| `Priority` | 5 | Mikser process'inin önceliği |
| `Debug` | 0 | 1: `T:openal.log` dosyasına log yazar |

### Proje hakkında

Bu kütüphanenin kodu, Anthropic'in yapay zekâ kodlama asistanı
[Claude Code](https://claude.com/claude-code) ile birlikte, gerçek
donanım üzerinde çalışılarak yazıldı: derlemeleri Amiga'mda çalıştırıp
sonuçları geri gönderdim, Claude Code kodu yazdı, sorunları buldu ve
düzeltti. RTCW portumda olduğu gibi bu da bu çalışma şeklinin ne kadar
ileri gidebileceğini görmek için bir deneme.

### Lisans

zlib lisansı, bkz. [LICENSE](LICENSE).
