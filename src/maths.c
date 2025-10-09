#include "maths.h"

float lerp_a_b(float a, float b, float ratio) {
	if (ratio > 1) return b;
	if (ratio < 0) return a;
	return a * ratio + b * (1 - ratio);
}