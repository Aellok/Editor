#pragma once
#include "DX12Pipeline.h"
#include "DX12UAV.h"

struct DX12Compute
{
	DX12UAV UAV;
	DX12Pipeline pipeline;
	void Initialize(DirectX12* DX12, const s8* Shader);
	void Execute(DX12CommandQueue* Queue);
};