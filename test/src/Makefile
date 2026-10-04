# The test programs, built against the SDK.
#
#   make            with gcc (bebbo's m68k-amigaos-gcc), using openal.library
#   make vbcc       with vbcc, using openal.library
#
# The programs in test/ were built with "make".

SDK   = ../../SDK
TESTS = al_selftest al_info al_playwav al_tone3d al_bench al_libtest

CC      = m68k-amigaos-gcc
CFLAGS  = -O2 -noixemul -m68040 -fno-math-errno -Wall -I$(SDK)/include
LIBS    = -L$(SDK)/gcc -lopenalshared -lm

VC      = vc
VFLAGS  = -c99 -cpu=68040 -fpu=68040 -O=1 -I$(SDK)/include
VLIBS   = $(SDK)/vbcc/openal.lib -lm040 -lamiga

all: $(TESTS) al_mathcheck

al_selftest: selftest.c common.h
	$(CC) $(CFLAGS) selftest.c -o $@ $(LIBS)
al_info: info.c common.h
	$(CC) $(CFLAGS) info.c -o $@ $(LIBS)
al_playwav: playwav.c common.h
	$(CC) $(CFLAGS) playwav.c -o $@ $(LIBS)
al_tone3d: tone3d.c common.h
	$(CC) $(CFLAGS) tone3d.c -o $@ $(LIBS)
al_bench: bench.c common.h
	$(CC) $(CFLAGS) bench.c -o $@ $(LIBS)
al_libtest: libtest.c
	$(CC) $(CFLAGS) libtest.c -o $@
al_mathcheck: mathcheck.c
	$(CC) $(CFLAGS) mathcheck.c -o $@ -lm

vbcc:
	mkdir -p vbcc
	for t in selftest info playwav tone3d bench; do \
		$(VC) $(VFLAGS) $$t.c -o vbcc/al_$$t $(VLIBS) || exit 1; \
	done

clean:
	rm -rf $(TESTS) al_mathcheck vbcc

.PHONY: all vbcc clean
