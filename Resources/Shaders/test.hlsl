#include "GLOBAL.hlsl"

struct struturedParam
{
    float4 color;
};

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
    //float3 normal : NORMAL;
    uint vertexID : SV_VertexID;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float4 paramColor : COLOR;
};

Texture2D g_tex_0 : register(t0);
StructuredBuffer<struturedParam> Structured : register(t1);
SamplerState g_sam_0 : register(s0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.0f), WorldMat);
    output.pos = mul(output.pos, VPMatrix);
    output.paramColor = Structured[input.vertexID].color;
    
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 color = g_tex_0.Sample(g_sam_0, input.uv);

    // 버텍스 셰이더에서 전달된 색상 더하기
    color += input.paramColor;

    return color;
    
}
