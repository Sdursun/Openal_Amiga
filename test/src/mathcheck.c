/*
 * Checks the math functions the mixer relies on, printed as integers
 * (value * 1000) because libnix's printf gets some floats wrong.
 */

#include <stdio.h>
#include <math.h>

static volatile double one = 1.0, two = 2.0, half = 0.5, minus_one = -1.0;
static volatile float fhalf = 0.5f, fzero = 0.0f, ften = 10.0f;

static void show(const char *what, double v, long expect)
{
	long got = (long)(v * 1000.0 + (v < 0 ? -0.5 : 0.5));

	printf("%-28s %8ld  (expected %8ld)%s\n", what, got, expect, got == expect ? "" : "  <-- WRONG");
}

int main(void)
{
	float a, b;

	show("sqrt(2)", __builtin_sqrt(two), 1414);
	show("atan2(1, -1)", atan2(one, minus_one), 2356);
	show("atan2(0, -10) [float args]", atan2((double)fzero, (double)-ften), 3142);
	show("acos(0.5)", acos(half), 1047);
	show("pow(2, -1)", pow(two, minus_one), 500);
	show("cos(0.5)", cos(half), 878);

	/* float arithmetic around a call, as in the mixer */
	a = fhalf * 3.0f;
	b = (float)atan2((double)fhalf, (double)-ften);
	show("float after atan2 (1.5)", a, 1500);
	show("atan2(0.5, -10)", b, 3092);
	return 0;
}
