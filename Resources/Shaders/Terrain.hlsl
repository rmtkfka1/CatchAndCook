#include "GLOBAL.hlsl"
#include "Light.hlsl"

cbuffer test : register(b1)
{
    row_major matrix WorldMat;
}

cbuffer cameraParams : register(b2)
{
    row_major Matrix ViewMatrix;
    row_major Matrix ProjectionMatrix;
    row_major Matrix VPMatrix;
    row_major Matrix InvertViewMatrix;
    row_major Matrix InvertProjectionMatrix;
    row_major Matrix InvertVPMatrix;

    float4 cameraPos;
    float4 cameraLook;
    float4 cameraUp;
    float4 cameraFrustumData;
    float4 cameraScreenData;
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
    float4 worldPos : POSITIONT;
    float4 worldNormal : NORMAL;
    float2 uv : TEXCOORD0;

};

Texture2D g_tex_0 : register(t0);
Texture2D heightMap : register(t1);
SamplerState g_sam_1 : register(s1);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float Height = heightMap.SampleLevel(g_sam_1, input.uv, 0).r;
    output.pos = mul(float4(input.pos, 1.0f), WorldMat);
    output.pos.y += Height *1000;
    output.worldNormal = mul(float4(input.normal, 0.0f), WorldMat);
    
    output.pos = mul(output.pos, VPMatrix);

    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

    return float4(1, 1, 1, 1);
   
}
