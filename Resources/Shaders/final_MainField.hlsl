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

Texture2D _BakedGIMap : register(t8);

LightingResult ComputeLightColorDeferred(float3 worldPos ,float3 WorldNomral)
{
    float3 lightColor = float3(0, 0, 0);

    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    LightingResult result = (LightingResult)0;

    [unroll]
    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].onOff == 1)
        {
	        if (g_lights[i].mateiral.lightType == 0)
	        {
	            lightColor += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, worldPos, WorldNomral, toEye, result);
	        }
	        else if (g_lights[i].mateiral.lightType == 1)
	        {
	            lightColor += ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
	        }
	        else if (g_lights[i].mateiral.lightType == 2)
	        {
	            lightColor += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
	        }
		}
    }
    
    return result;
}


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.f);
    output.uv = input.uv;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float4 worldPos = PositionTexture.Sample(sampler_point, input.uv);
    float3 WolrdNormal = normalize(NormalTexture.Sample(sampler_point, input.uv).xyz);
    float4 AlbedoColor = AlbedoTexture.Sample(sampler_lerp, input.uv);

    LightingResult lightColor = ComputeLightColorDeferred(worldPos.xyz, WolrdNormal.xyz);
    float4 ShadowColor = _BakedGIMap.Sample(sampler_lerp, saturate(dot(float3(0, 1, 0), WolrdNormal) * 0.5 + 0.5));

    float3 finalColor = (lerp(ShadowColor * AlbedoColor, AlbedoColor, lightColor.atten) + float4(lightColor.subColor, 0)).xyz;
    
    return float4(finalColor, 1.0f);
};