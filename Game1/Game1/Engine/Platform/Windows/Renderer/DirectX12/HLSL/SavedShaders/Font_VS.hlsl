CBV cbuffer OnResize : register(b0)
{
    matrix OMat;
}
CBV cbuffer PerFrame : register(b1)
{
    matrix MMat;
    float4 Color;
}
VS_INPUT struct VertexPosColor
{
    float4 Position : POSITION0;
    float2 UV : TEXCOORD0;
    float2 UVPercent : INSTANCE_UV;
    column_major float4x4 model : INSTANCE_TRANSFORM;
    float Index : INSTANCE_INDEX;
};
VS_OUTPUT struct PixelShaderInput
{
    float4 Position : SV_POSITION;
    float2 UV : TEXCOORD0;
    float4 Color : COLOR0;
    float index : TEXCOORD1;
};
PixelShaderInput main(VertexPosColor vertIn)
{
    PixelShaderInput psInput;

    psInput.Position = mul(vertIn.Position, vertIn.model);
    psInput.Position = mul(psInput.Position, OMat);

    psInput.UV = vertIn.UV * vertIn.UVPercent;
    psInput.Color = Color;
    psInput.index = vertIn.Index;
    return psInput;
}