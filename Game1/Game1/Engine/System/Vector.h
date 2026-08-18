#pragma once
#include <DirectXMath.h>
#include "System\Types.h"
typedef DirectX::XMVECTOR Vector;
typedef DirectX::XMMATRIX Matrix;

Matrix GetTranslationMatrix(Vector Pos);
Matrix GetScaleMatrix(Vector Dim);
Matrix GetRotationMatrix(Vector Rot);

Matrix GetAlignmentRotation_3D(Vector target, Vector start);

f32 GetAlignmentRotation_2D(Vector target, Vector start);
