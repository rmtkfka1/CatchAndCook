#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
};

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    
    output.pos = mul(float4(input.pos, 1.0f), data.localToWorld);
    output.worldPos = output.pos;
    output.pos = mul(output.pos, VPMatrix);
  
    output.normal = mul(float4(input.normal, 0.0f), data.localToWorld);
    
    output.color = input.color;
    
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 color;
    
    float4 worldPos = input.worldPos;
    
    float3 WolrdNormal = normalize(input.normal);
    
    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, toEye);
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
    
    return float4(color, 1.0f) * input.color;
}
