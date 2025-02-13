#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

#include "Light_b3.hlsl"


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
SamplerState g_sam_0 : register(s0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = mul(float4(input.pos, 1.0f), LocalToWorldMatrix);
    
    output.worldPos = output.pos;
    output.worldNormal = mul(float4(input.normal, 0.0f), LocalToWorldMatrix);
    
    output.pos = mul(output.pos, VPMatrix);

    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

    float3 color;
    
    float4 worldPos = input.worldPos;
    float4 WolrdNormal = input.worldNormal;
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

    return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
    

}
