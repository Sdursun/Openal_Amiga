# openal.library geliştirici rehberi

openal.library, AmigaOS 3.x (68k) üzerinde OpenAL 1.1 sağlar. OpenAL için
yazılmış programlar `SDK/include` başlıklarıyla değişiklik yapılmadan
derlenir. Bu rehber kütüphaneye nasıl bağlanılacağını, kütüphanenin nasıl
çağrıldığını ve oyun portlarken bilinmesi gerekenleri anlatır.

## Gereksinimler

- AmigaOS 3.x
- FPU'lu bir 68040 veya 68060, ya da Emu68 kullanan bir makine (PiStorm).
  Kütüphane `-m68040` ile derlenmiştir.
- AHI ve kullanılan birim için seçilmiş bir mod (`ENV:OpenAL/Unit`
  ayarlanmadıysa birim 0)

## SDK içeriği

```
SDK/include/AL/           al.h, alc.h, alext.h: OpenAL API'si
SDK/include/libraries/    openal_dispatch.h: kütüphanenin fonksiyon tablosu
SDK/include/proto/        openal.h: derleyicinize uygun dosyayı seçer
SDK/include/clib/         openal_protos.h
SDK/include/inline/       openal.h (gcc), openal_protos.h (vbcc)
SDK/include/pragmas/      openal_pragmas.h (SAS/C)
SDK/fd, SDK/sfd           openal_lib.fd, openal_lib.sfd
SDK/gcc/libopenalshared.a openal.library stub'ları, gcc
SDK/gcc/libopenal.a       OpenAL'ın tamamı, statik kütüphane, gcc
SDK/vbcc/openal.lib       openal.library stub'ları, vbcc
SDK/sasc/                 SAS/C için stub kaynakları ve smakefile
```

## Bağlama

Normal OpenAL kodu yazılır; hangi kütüphanenin kullanılacağı bağlama
sırasında seçilir.

**gcc (bebbo m68k-amigaos-gcc), openal.library ile:**

```
m68k-amigaos-gcc -noixemul -m68040 -ISDK/include -c oyun.c
m68k-amigaos-gcc -noixemul -m68040 oyun.o -o oyun -LSDK/gcc -lopenalshared
```

**gcc, statik** (OpenAL programın içine girer, openal.library gerekmez):

```
m68k-amigaos-gcc -noixemul -m68040 oyun.o -o oyun -LSDK/gcc -lopenal
```

**vbcc:**

```
vc -c99 -cpu=68040 -fpu=68040 -ISDK/include oyun.c -o oyun SDK/vbcc/openal.lib -lamiga
```

**SAS/C 6.x:** Önce `SDK/sasc` içindeki `smakefile` ile `openal.lib`
derlenir, sonra:

```
sc LINK oyun.c IDIR=SDK/include LIB SDK/sasc/openal.lib
```

SAS/C dosyaları henüz derlenmedi, çünkü elimizde SAS/C yoktu. gcc ve
vbcc kütüphaneleri test edildi.

## Program kütüphaneyi nasıl kullanır

Stub'lar (`libopenalshared.a`, `openal.lib`) bütün OpenAL fonksiyonlarını
içerir. İlk çağrı `openal.library`'yi açar, `atexit` ise program çıkarken
kapatır. Kütüphane açılamazsa hiçbir çağrı bir şey yapmaz ve
`alcOpenDevice` NULL döner. Böylece oyun hata vermek yerine sessiz devam
eder.

Program çıkarken açık bıraktığı her şeyi (cihazlar, mikser task'ı, AHI)
kütüphane kapatır.

Birden fazla program kütüphaneyi aynı anda kullanabilir. Her birinin
kendi geçerli context'i ve hata durumu vardır. Tüm programlar tek bir
mikser task'ı ve tek bir AHI akışı üzerinden çalar; AHI birimi tek
kanallı olsa da sesler birlikte duyulur.

## Kütüphane nasıl çağrılır

Programların bunu bilmesi gerekmez. Kendi stub'larınızı ya da başka bir
dil için bağlantı yazacaksanız önemlidir.

Kütüphanenin standart dört fonksiyon dışında tek bir giriş noktası
vardır:

```
const struct OALDispatch *OALGetDispatch(ULONG abi)   /* d0, LVO -30 */
```

Bu fonksiyon C fonksiyon işaretçilerinden oluşan bir tablo döndürür
(`SDK/include/libraries/openal_dispatch.h`). Kütüphane o ABI sürümünü
desteklemiyorsa NULL döner. `OAL_ABI_VERSION` ile çağrılır. Ardından:

1. `oal_create_handle()` bir kez çağrılır. Dönen handle, tablodaki her
   fonksiyona ilk parametre olarak verilir.
2. `CloseLibrary()`'den önce `oal_destroy_handle(handle)` çağrılır.

Her derleyiciden çağrılabilmesi için tablodan yalnızca 32 bit tam
sayılar ve işaretçiler geçer:

- **Float ve double parametreler** işaretçiyle geçirilir; derleyiciler
  float'ın yığına nasıl konacağında anlaşamıyor.
- **Float ve double sonuçlar** son parametre olarak verilen bir işaretçi
  üzerinden döner; derleyiciye göre `fp0`'da ya da `d0`'da olurlardı.
- **ALboolean sonuçlar** ALint olarak döner.
- **Tüm parametreler yığından geçer.** SAS/C için `OAL_CALL`,
  `__stdargs` ekler.

`oal_count` kütüphanedeki fonksiyon sayısını verir. Fonksiyonlar tabloya
yalnızca sondan eklenir. Bu yüzden bir program, `oal_count` değeri kendi
derlendiği `OAL_FUNC_COUNT`'tan küçük olmayan her kütüphane sürümüyle
çalışır.

## Port notları

- **OpenAL'ı çalışma anında yükleyen motorlar:** ioquake3 tabanlı
  oyunlar OpenAL'ı `dlopen` / `Sys_LoadDll` ile yükler (`qal.c`). Bu kod
  fonksiyonları doğrudan çağıracak ya da `alGetProcAddress` /
  `alcGetProcAddress` ile alacak şekilde değiştirilmelidir. Stub'larla bu
  iki fonksiyon stub fonksiyonlarını döndürür ve context açılmadan önce
  de çalışır.
- **Cihaz adları:** `alcOpenDevice(NULL)`, `"AHI"` ve bilinen Windows
  cihaz adları AHI çıkışını açar.
- **16 bit ses verisi** OpenAL'ın tanımladığı gibi makinenin bayt
  sırasındadır (big-endian). WAV dosyaları little-endian'dır;
  `alBufferData`'dan önce baytlar çevrilmelidir (`test/src/common.h`).
- **MiniGL ekranı tutarken konsol çıktısı yok:** Kütüphane konsola hiç
  yazmaz; debug logu `T:openal.log` dosyasına gider. `minigl.library`
  ekranı tutarken konsola yazmak makineyi kilitleyebilir.
- **Task'lar:** OpenAL herhangi bir task'tan çağrılabilir. Kütüphane
  belleği `AllocVec` ile alır.
- **Yapı hizalaması:** API'de yapı (struct) yoktur; `-malign-int` ile
  ya da onsuz derlenmiş programlarla çalışır.

### gcc 6.5 ve -m68040

bebbo m68k-amigaos-gcc 6.5, `-m68040` ile bazı FPU kodlarını yanlış
derler. Tek hassasiyete yuvarlayan bir komut `(aN)+` ile bir double
okuduğunda, gcc yazmacın 4 bayt ilerlediğini varsayar; CPU ise 8 bayt
ilerletir. O yazmaçla yapılan sonraki her erişim 4 bayt kayık olur.
openal-amiga'da bu hata `alSource3f`'in y ve z değerlerini çöpe
çeviriyordu. Kendi kodunuzun `gcc -S` çıktısında şu kalıba bakın:

```
fsmove.d (a3)+,fp0      v[0]'ı okur; a3 += 8
fsmove.d (4,a3),fp0     v[1] olmalıydı, o (0,a3)'te
```

Kütüphane ve SDK her derlemede bu kalıba karşı taranır.

## Desteklenen özellikler

- OpenAL 1.1'in tamamı: kaynaklar, buffer'lar, dinleyici, tüm mesafe
  modelleri, koni, doppler, offset'ler, buffer kuyruğuyla akış
- Formatlar: mono ve stereo, 8 bit, 16 bit, 32 bit float
  (`AL_EXT_FLOAT32`)
- Uzantılar: `AL_EXT_FLOAT32`, `AL_EXT_OFFSET`,
  `AL_EXT_LINEAR_DISTANCE`, `AL_EXT_EXPONENT_DISTANCE`,
  `ALC_ENUMERATION_EXT`, `ALC_ENUMERATE_ALL_EXT`, `ALC_SOFT_loopback`
- Stereo çıkış; panning, openal-soft'un varsayılan stereo çıkışından
  ölçülerek ona uydurulmuştur
- Desteklenmeyenler: kayıt (capture), EFX, HRTF, ikiden fazla çıkış
  kanalı

## Ayarlar

`ENV:OpenAL/` içindeki ortam değişkenleri (önce `MakeDir ENV:OpenAL`):

| Değişken | Varsayılan | Anlamı |
|---|---|---|
| `Frequency` | 22050 | Miksleme hızı (Hz) |
| `Period` | 1024 | Çıkış buffer'ı başına frame; gecikme yaklaşık iki period |
| `Unit` | 0 | AHI birimi, 0-3 |
| `Priority` | 5 | Mikser process'inin önceliği |
| `Debug` | 0 | 1: `T:openal.log` dosyasına log yazar |
