#include "ShaderDefines.hlsli"

CBV cbuffer OnResize : register(b0)
{
    matrix VMat;
    matrix PMat;

    float4 CameraPosition;
}
CBV cbuffer PerFrame : register(b1)
{
    matrix MMat;
};

struct VertexPosColor
{
    INPUT float4 Position : POSITION0;
    INPUT float2 UV       : TEXCOORD0;
    INPUT float3 Normal   : NORMAL0;
};
struct PixelShaderInput
{
    OUTPUT float4 Position : SV_POSITION;
    OUTPUT float2 UV : TEXCOORD0;
    OUTPUT float3 Normal : NORMAL0;

    OUTPUT float4 ModelPosition : POSITION0;
    OUTPUT float4 ViewDir : POSITION1;
};
PixelShaderInput main(VertexPosColor vertIn)
{
    PixelShaderInput psInput;
    psInput.ModelPosition = mul(vertIn.Position, MMat);
    
    psInput.Position      = mul(psInput.ModelPosition, VMat);
    psInput.Position      = mul(psInput.Position, PMat);

    psInput.Normal = mul(vertIn.Normal, (float3x3)MMat);
    psInput.Normal = normalize(psInput.Normal);

    psInput.ViewDir = normalize(CameraPosition - psInput.ModelPosition);

    psInput.UV = vertIn.UV;
	return psInput;
}