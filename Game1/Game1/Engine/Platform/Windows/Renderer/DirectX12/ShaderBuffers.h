#pragma once
#include "../../../../System/Types.h"
#include "../../../../System/Vector.h"

//NOTE: All these need to be 256 aligned
struct DefaultOnResizeBuffer
{
	Matrix View; // 64
	Matrix Proj; // 128

	Vector CamPos; // 144

	f32 pad[28];// 256
};
struct DefaultPerFrameBuffer
{
	Matrix Model; // 64
	f32 pad[48]; // 256
};

struct Object2DPerFrameBuffer
{
	Matrix Model; // 64
	Vector Color; // 16
	f32 pad[44]; // 240
};
struct Object2DOnResizeBuffer
{
	Matrix Ortho; // 64
	f32 pad[48]; // 240
};