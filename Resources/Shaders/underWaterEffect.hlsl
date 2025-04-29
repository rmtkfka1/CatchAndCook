SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_shadow : register(s5);
SamplerState sampler_lerp_clamp : register(s6);

#include "Light_b3.hlsl"


cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
};

cbuffer underWatgerParam : register(b1)
{
    float3 g_fogColor;
    float g_fog_power;

    float3 g_underWaterColor;
    float g_fogMin;

    float2 padding;
    int g_on; 
    float g_fogMax; 
};

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

RWTexture2D<float4> resultTexture : register(u0);
Texture2D depthT : register(t0);
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> PositionTexture : register(t2);
Texture2D<float4> NormalTexture : register(t3);
Texture2D<float4> MAOTexture : register(t4);


float3 ProjToView(float2 texCoord)
{
    float4 posProj;
    // texCoord는 픽셀 좌표이므로 정규화 좌표로 변환
    posProj.xy = (texCoord + 0.5f) / cameraScreenData.xy * 2.0f - 1.0f;
    posProj.y *= -1; 
    posProj.z = depthT.Load(int3(texCoord, 0));
    posProj.w = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    return posView.xyz/posView.w;
}

float CalculateFogFactor(float3 posView)
{
    float dist = length(posView.xyz);
    float distFog = saturate((dist - g_fogMin) / (g_fogMax - g_fogMin));
    float fogFactor = exp(-distFog * g_fog_power);
    return 1-fogFactor;
}

///////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    int2 tex = id.xy;
    float2 uv = (float2(tex) + 0.5) / cameraScreenData.xy;

    // 기본 G-buffer 샘플링
    float3 albedo = RenderT.SampleLevel(sampler_lerp, uv, 0).xyz;
    float3 normal = normalize(NormalTexture.SampleLevel(sampler_point, uv, 0).xyz);
    float3 worldPos = PositionTexture.SampleLevel(sampler_point, uv, 0).xyz;
    float3 viewPos = ProjToView(tex);

    // 1) 거리 기반 fogFactor (멀수록 1)
    float fogFactor = CalculateFogFactor(viewPos);

    // 2) 라이트 계산 & 뎁스 노멀라이즈
    LightingResult light = ComputeLightColor(worldPos, normal);
    float distFogNorm = saturate((viewPos.z - g_fogMin) / (g_fogMax - g_fogMin));

    float swAtten = lerp(light.subWaterAtten, 1.0f, distFogNorm);
    
    float3 underCol = lerp(g_underWaterColor * albedo, albedo, light.atten);

    float3 litColor = underCol + light.subColor;

    float adjustedFog = saturate(fogFactor * swAtten);
    float3 finalColor = lerp(litColor, g_fogColor, adjustedFog);

    resultTexture[tex] = float4(finalColor, 1.0f);
}