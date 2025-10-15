#include "maths.h"

float lerp_a_b(float a, float b, float t) {
	if (t > 1) return b;
	if (t < 0) return a;
	return a + t * (b - a);
}