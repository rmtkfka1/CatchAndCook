
#define SKINNED
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Skinned_b5.hlsl"


cbuffer DefaultMaterialParam : register(b7)
{
	float4 color;
	float4 _baseMapST;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 boneIds : BONEIDs;
    float4 boneWs : BONEWs;
};

struct VS_OUT
{
    float4 positionOS : SV_Position;
    float4 positionWS : SV_Position;
    float3 normalOS : NORMAL;
    float3 normalWS : NORMAL;
    float2 uv : TEXCOORD0;
};

Texture2D _BaseMap : register(t0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.positionOS = TransformLocalToWorld(float4(input.pos, 1.0f), input.boneIds, input.boneWs);
    output.positionWS = TransformWorldToClip(output.positionOS);
    output.normalOS = input.normal;
    output.normalWS = TransformNormalLocalToWorld(output.normalOS);
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   return _BaseMap.Sample(sampler_lerp, input.uv) * color;
}
