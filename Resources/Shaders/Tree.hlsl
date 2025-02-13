#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

cbuffer DefaultMaterialParam : register(b7)
{
	float4 color;
	float4 _baseMapST;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

Texture2D _BaseMap : register(t0);
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.0f), LocalToWorldMatrix);
    output.pos = mul(output.pos, VPMatrix);

    output.uv = input.uv * _baseMapST.xy + _baseMapST.zw;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return _BaseMap.Sample(g_sam_0, input.uv) * color;
}
