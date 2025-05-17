#include "Light_b3.hlsl"

SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_shadow : register(s5);
SamplerState sampler_lerp_clamp : register(s6);

cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
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


cbuffer ScatterParams : register(b5)
{
    float phaseG;
    float absorption;
    float2 Padding;

    float density;
    float3 scatterColor;

    float numSteps;
    float stepSize;
    float2 padding2;
    //float3 MainlightPos;
    //float padding2;
};

cbuffer underWatgerParam : register(b6)
{
    float3 g_fogColor;
    float g_fog_power;

    float3 g_underWaterColor;
    float g_fogMin;

    float2 padding;
    int g_on;
    float g_fogMax;
};

Texture2D<float4> sceneColor : register(t0);
Texture2D<float> DepthTexture : register(t1);
RWTexture2D<float4> Output : register(u0);

float3 ReconstructViewPos(int2 pixel)
{
    float2 ndc = (pixel + 0.5f) / cameraScreenData.xy * 2.0f - 1.0f;
    ndc.y *= -1;
    float rawDepth = DepthTexture.Load(int3(pixel, 0));
    float clipZ = rawDepth * 2.0f - 1.0f; 
    float4 posProj = float4(ndc, clipZ, 1.0f);
    float4 viewH = mul(posProj, InvertProjectionMatrix);
    return viewH.xyz / viewH.w;
}



[numthreads(16, 16, 1)]
void CS_Main(uint3 id : SV_DispatchThreadID)
{
    int2 pix = int2(id.xy);

    float3 baseColor = sceneColor[pix].rgb;
    float3 vPosVS = ReconstructViewPos(pix);
    float maxDist = length(vPosVS);
    float3 viewDirVS = vPosVS / maxDist; 

    float3 lightDirVS = normalize(mul((float3x3) ViewMatrix, mainLight.direction));
    float3 accScatter = float3(0, 0, 0);
    float transmittance = 1.0f;
    
    for (int i = 0; i < numSteps; ++i)
    {
        float t = (i + 0.5f) * stepSize;
        float3 samplePos = viewDirVS * t;

        float cosTheta = dot(-viewDirVS, lightDirVS);
        float g2 = phaseG * phaseG;
        float denom = max(1 + g2 - 2 * phaseG * cosTheta, 1e-4);
        float phase = (1 - g2) / (4 * 3.14159265 * pow(denom, 1.5));

        float absorbT = exp(-absorption * t);

        float3 scatter = scatterColor * phase * absorbT
                              * density * transmittance * stepSize;
        accScatter += scatter;


        transmittance *= exp(-absorption * stepSize);
    }

    float3 litColor = baseColor + accScatter;
    float distFog = saturate((maxDist - g_fogMin) / (g_fogMax - g_fogMin));
    float fogFactor = 1 - exp(-distFog * g_fog_power);

    float3 finalColor = lerp(litColor, g_underWaterColor, fogFactor);

    Output[pix] = float4(finalColor, 1);
}


//[numthreads(16, 16, 1)]
//void CS_Main(uint3 id : SV_DispatchThreadID)
//{
//    int2 pixel = id.xy;
//    float3 baseColor = FoggedScene[pixel].rgb;

//    float3 viewPos = ReconstructViewPos(pixel);
//    float3 worldPos = mul(float4(viewPos, 1.0f), InvertViewMatrix).xyz;

//    //viewDir: 픽셀에서 카메라(또는 관찰자) 방향으로 향하는 단위 벡터  
//    //lightDir: 픽셀에서 빛(광원) 방향으로 향하는 단위 벡터
    
//    float3 lightToPixel = normalize(mainLight.position - worldPos ); // 픽셀에서 광원으로 나가는방향
//    float3 lightDir = normalize(-mainLight.direction);  //픽셀에서 광원으로 향하는방향

//    float cosTheta = dot(lightToPixel, lightDir);
//    float g2 = phaseG * phaseG;
//    float denom = max(1 + g2 - 2 * phaseG * cosTheta, 1e-3);
//    float phase = (1 - g2) / (4 * 3.14159 * pow(denom, 1.5));

//    float lightDepth = length(worldPos - mainLight.position);
//    float atten = 1.0 / (1.0 + log(1.0 + absorption * lightDepth));

//    float3 scatter = scatterColor * phase * atten * density;
//    float3 finalColor = baseColor + scatter;
//    Output[pixel] = float4(finalColor, 1);
//}
