TEXTURE Texture2DArray t1 : register(t0);
SAMPLER SamplerState s1 : register(s0);

PS_INPUT struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 color : COLOR0;
	float index : TEXCOORD1;
};
float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 TextureColor = t1.Sample(s1, float3(input.UV.x, input.UV.y, input.index)) + input.color.rgba;
    return TextureColor;
}