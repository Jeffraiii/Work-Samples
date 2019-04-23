#include "Constants.hlsl"

#define MAX_SKELETON_BONES 80
cbuffer SkinConstants : register(b3)
{
    float4x4 c_skinMatrix[MAX_SKELETON_BONES];
};

struct VIn
{
    float3 position : POSITION0;
    float3 normal : NORMAL0;
    uint4 boneIndex : BONES0;
    float4 boneWeight : WEIGHTS0;
    float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
    float4 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

VOut VS(VIn vIn)
{
    VOut output;

#if 1
    float4 pos = float4(vIn.position, 1.0);
    vIn.position = mul(pos, c_skinMatrix[vIn.boneIndex.x]) * vIn.boneWeight.x
        + mul(pos, c_skinMatrix[vIn.boneIndex.y]) * vIn.boneWeight.y
        + mul(pos, c_skinMatrix[vIn.boneIndex.z]) * vIn.boneWeight.z
        + mul(pos, c_skinMatrix[vIn.boneIndex.w]) * vIn.boneWeight.w;
    float4 norm = float4(vIn.normal, 0.0);
    vIn.normal = mul(norm, c_skinMatrix[vIn.boneIndex.x]) * vIn.boneWeight.x
        + mul(norm, c_skinMatrix[vIn.boneIndex.y]) * vIn.boneWeight.y
        + mul(norm, c_skinMatrix[vIn.boneIndex.z]) * vIn.boneWeight.z
        + mul(norm, c_skinMatrix[vIn.boneIndex.w]) * vIn.boneWeight.w;
#endif

    // transform input position from model to world space
    float4 worldPos = mul(float4(vIn.position, 1.0), c_modelToWorld);
    // transform position from world to projection space
    output.position = mul(worldPos, c_viewProj);

    // transform input normal from model to world space
    output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);

    output.uv = vIn.uv;
    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
    float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);

    // do the lighting
    float3 lightColor = c_ambient;

    //TODO Fill This in
	float3 n = normalize(pIn.normal.xyz);
	float gradient = dot(normalize(c_dirLight.dir), n);
	if (gradient < 0.0) {
		gradient = 0.0;
	}
	if (gradient > 0.95) {
		lightColor += float3(1.0, 1.0, 1.0);
	}
	else if (gradient > 0.5) {
		lightColor += float3(0.7, 0.7, 0.7);
	}
	else if (gradient > 0.05) {
		lightColor += float3(0.35, 0.35, 0.35);
	}
	else {
		lightColor += float3(0.1, 0.1, 0.1);
	}
    float4 finalColor = diffuse * float4(lightColor, 1.0);
    return finalColor;
}
