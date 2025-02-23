#include "Global_b0.hlsl"
#include "Light_b3.hlsl"

//[쉐이더정리][컬링처리]

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.f);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   
  
    float3 color;
    
    float4 worldPos = PositionTexture.Sample(sampler_point, input.uv);
    float3 WolrdNormal = normalize(NormalTexture.Sample(sampler_point, input.uv).xyz);
    float4 AlbedoColor = AlbedoTexture.Sample(sampler_lerp, input.uv);
    float Depth = depthTexture.Sample(sampler_point, input.uv).r;
    
    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);
    
    for (int i = 0; i < g_lightCount; ++i)
    {
   
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, WolrdNormal.xyz, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, toEye);
        }
        else if (g_lights[i].mateiral.lightType == 2)
        {
            color += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, toEye);
        }
          
    }
    
    return float4(color, 1.0f) * AlbedoColor;
};