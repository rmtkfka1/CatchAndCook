
#ifndef SEA_GLOBAL
#define SEA_GLOBAL
#include "Global_b0.hlsl"

Texture2D globalT17 : register(t17);

float4 ComputeCaustics(float2 uv, float scale, float3 worldPos)
{

    float power = smoothstep(-800.0f, 50.0f, worldPos.y);


    float2 waveOffset = float2(
        sin(g_Time * 0.4f + uv.y * 10.0f),
        cos(g_Time * 0.6f + uv.x * 10.0f)
    ) * 0.05f;

    float2 causticUV = uv * scale + waveOffset;
    
    float3 caustic = globalT17.Sample(sampler_lerp, causticUV).rgb;

    return float4(caustic * 0.3f * power, 0.0f);
}


#endif