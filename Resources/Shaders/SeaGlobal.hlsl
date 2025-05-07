
#ifndef SEA_GLOBAL
#define SEA_GLOBAL
#include "Global_b0.hlsl"

Texture2D globalT17 : register(t17);

float4 ComputeCaustics(float2 uv , float scale, float3 worldPos)
{
    
    float power = smoothstep(-1000.0f,50.0f, worldPos.y);
 
    
    float2 scroll1 = uv * scale + g_Time * float2(0.02f, 0.01f);
    float2 scroll2 = uv * scale + g_Time * float2(-0.015f, 0.018f);
       
    float3 c1 = globalT17.Sample(sampler_lerp, scroll1).rgb;
    float3 c2 = globalT17.Sample(sampler_lerp, scroll2).rgb;

    float3 caustic = (c1 + c2) * 0.5f;

    return float4(caustic, 0) * power;
}


#endif