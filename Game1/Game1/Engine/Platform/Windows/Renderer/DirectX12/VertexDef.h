#pragma once
#include <DirectXMath.h>
#include "System\Types.h"
#include "System\Vector.h"
struct Vertex
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT3 Normal;
};

struct FontInstanceBuffer2D
{
	DirectX::XMFLOAT2 UVPercent;
	DirectX::XMFLOAT4X4 model;
	float Index;
};
struct Font2InstanceBuffer2D
{
	DirectX::XMFLOAT2 UVPercent;
	DirectX::XMFLOAT4X4 model;
	float Index;
	DirectX::XMFLOAT4 Color;
};
struct InstanceBuffer2D
{
	Matrix model;
	float Index;
};

struct Vertex2D
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT2 UV;
};
struct FontVertex2D
{
	DirectX::XMFLOAT4 Position;
	DirectX::XMFLOAT2 UV;
};