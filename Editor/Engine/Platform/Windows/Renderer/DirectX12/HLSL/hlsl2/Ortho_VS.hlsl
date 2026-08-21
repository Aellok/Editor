#include "ShaderDefines.hlsli"

CBV cbuffer OnResize : register(b0)
{
    matrix OMat;
}
CBV cbuffer PerFrame : register(b1)
{
    matrix MMat;
    float4 Color;
}
struct VertexPosColor
{
    INPUT float4 Position : POSITION0;
    INPUT float2 UV       : TEXCOORD0;
};
struct PixelShaderInput
{
    OUTPUT float4 Position : SV_POSITION;
    OUTPUT float2 UV       : TEXCOORD0;
    OUTPUT float4 Color    : COLOR0;
};
PixelShaderInput main(VertexPosColor vertIn)
{
    PixelShaderInput psInput;

    psInput.Position = mul(vertIn.Position, MMat);
    psInput.Position = mul(psInput.Position, OMat);

    psInput.UV = vertIn.UV;
    psInput.Color = Color;
    return psInput;
}