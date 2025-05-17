
#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"


cbuffer VolumetricData : register(b5)
{
    float3 fogColor;
    float phaseG;
    
    float3 lightDir;
    float waterHeight;
    
    float absorption;
    int numSlices;
    int numSteps;
    float stepSize;
};


struct VS_OUT
{
    float4 pos : SV_POSITION;
    float3 ndcPos : POSITION;
};

VS_OUT VS_Main(uint vid : SV_VertexID, uint instanceID : SV_InstanceID)
{
    VS_OUT o;

    float2 verts[4] =
    {
        float2(-1, 1), float2(1, 1), 
        float2(-1, -1), float2(1, -1)
    };
    

    o.pos = float4(verts[vid], 0.0f, 1.0f);
    float ndcZ = (instanceID + 0.5f) / numSlices;
    o.ndcPos = float3(o.pos.xy, ndcZ);

    return o;
}


float3 ProjToView(float3 ndcPos)
{
    float4 posProj;
    
    posProj.xy = ndcPos.xy;
    posProj.z  = ndcPos.z;
    posProj.w  = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    return posView.xyz / posView.w;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 viewPos = ProjToView(input.ndcPos);
    float depth = viewPos.z;


    float normDepth = saturate(depth / waterHeight);

    // 2) 포그 색상 → 멀수록 진해짐
    float3 fogCol = fogColor * normDepth;

    // 3) 알파도 멀수록 증가 (깊이 기반 가중치)
    float sliceAlpha = (1.0f - exp(-absorption * stepSize)) * normDepth;

    // 4) Premultiplied Alpha 출력
    return float4(fogCol * sliceAlpha, sliceAlpha);
}