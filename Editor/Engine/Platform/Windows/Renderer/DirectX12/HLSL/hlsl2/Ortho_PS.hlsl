#include "ShaderDefines.hlsli"

TEXTURE Texture2D t1 : register(t0);
SAMPLER SamplerState s1 : register(s0);

struct PixelShaderInput
{
	INPUT float4 position : SV_POSITION;
	INPUT float2 UV		: TEXCOORD0;
	INPUT float4 color	: COLOR0;
};
float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 TextureColor = t1.Sample(s1, input.UV) + input.color.rgba;
	
	return TextureColor;
}