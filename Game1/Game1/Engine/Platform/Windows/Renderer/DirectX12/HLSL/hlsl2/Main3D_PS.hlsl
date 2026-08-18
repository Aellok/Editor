#include "ShaderDefines.hlsli"

TEXTURE Texture2D t1 : register(t0);
SAMPLER SamplerState s1 : register(s0);

struct PixelShaderInput
{
	INPUT float4 position : SV_POSITION;
	INPUT float2 UV		: TEXCOORD0;
	INPUT float3 Normal	: NORMAL0;

	INPUT float4 ModelPos : POSITION0;
	INPUT float3 ViewDir  : POSITION1;
};
float4 main(PixelShaderInput input) : SV_TARGET
{
	//return float4(0.5f + (input.Normal.x / 2.0f),0.5f + (input.Normal.y / 2.0f),0.5f + (input.Normal.z / 2.0f),1); // ret normal
	
	float3 LightPosition = float3(0,0,0); // TODO: move these into there own buffer
	float3 LightColor	 = float3(1, 1, 1); //

	float4 Color = t1.Sample(s1, input.UV);

	float3 Intensities = float3(0.1, 0.8, 0.04);

	float3 Ambient = Intensities.x * LightColor;
	
	//Diffuse
	float3 LightDir = normalize(LightPosition - input.ModelPos);
	float LightAngle = dot(input.Normal, LightDir);
	
	// hidden
	if (LightAngle < 0)
	{
		return float4(Color.rgb * Ambient, Color.w);
	}

	float3 Diffuse = LightAngle * (Intensities.y * LightColor);
	//Specular
	float3 ReflectDir = reflect(-LightDir, input.Normal);
	float3 Specular = pow(saturate(dot(input.ViewDir,ReflectDir)),2) * (Intensities.z * LightColor);
	
	float3 PhongLighting = Color.rgb * (Ambient + Diffuse + Specular);

	return float4(PhongLighting, Color.w);
	
}