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
	float offset[3] = { 0.0, 1.3846153846, 3.2307692308 };	
	float weight[3] = { 0.2270270270, 0.3162162162, 0.0702702703 };

	float4 FragmentColor = DiffuseTexture.Sample(DefaultSampler, pIn.uv) * weight[0];
	for (int i = 1; i < 3; i++) {
		FragmentColor += DiffuseTexture.Sample(DefaultSampler, pIn.uv + float2(0, offset[i]) / 150) * weight[i];
		FragmentColor += DiffuseTexture.Sample(DefaultSampler, pIn.uv - float2(0, offset[i]) / 150) * weight[i];
	}
	return FragmentColor;
}