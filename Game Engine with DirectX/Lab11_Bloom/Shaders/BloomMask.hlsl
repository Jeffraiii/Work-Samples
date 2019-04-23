#include "Constants.hlsl"

struct VIn
{
	float3 position : POSITION0;
	float2 uv : UV0;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 uv : UV0;
};

VOut VS(VIn vIn)
{
	VOut output;
	output.position = float4(vIn.position, 1.0f);
	output.uv = vIn.uv;
	return output;
}


float4 PS(VOut pIn) : SV_TARGET
{
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
	if (!(diffuse.x >= 0.8 || diffuse.y >= 0.8f || diffuse.z >= 0.8f)) {
		diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	return diffuse;
}