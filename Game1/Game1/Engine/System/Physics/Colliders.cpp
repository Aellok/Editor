#include "Colliders.h"
bool Collider_SphereRay(Vector Orig, Vector Dir, f32 Radius)
{
	float a, b, c, discriminant;
	// Calculate the a, b, and c coefficients.
	a = (Dir.m128_f32[0] * Dir.m128_f32[0]) + (Dir.m128_f32[1] * Dir.m128_f32[1]) + (Dir.m128_f32[2] * Dir.m128_f32[2]);
	b = ((Dir.m128_f32[0] * Orig.m128_f32[0]) + (Dir.m128_f32[1] * Orig.m128_f32[1]) + (Dir.m128_f32[2] * Orig.m128_f32[2])) * 2.0f;
	c = ((Orig.m128_f32[0] * Orig.m128_f32[0]) + (Orig.m128_f32[1] * Orig.m128_f32[1]) + (Orig.m128_f32[2] * Orig.m128_f32[2])) - (Radius * Radius);

	// Find the discriminant.
	discriminant = (b * b) - (4 * a * c);

	// if discriminant is negative the picking ray missed the sphere, otherwise it intersected the sphere.
	if (discriminant < 0.0f)
	{
		return false;
	}

	return true;
}