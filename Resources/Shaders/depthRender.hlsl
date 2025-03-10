SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_shadow : register(s5);
SamplerState sampler_lerp_clamp : register(s6);

cbuffer FogParam : register(b1)
{
    float3 g_fogColor;
    float power;
    
    float g_fogMin;
    float g_fogMax;
    int g_depthRendering;
    float padding;
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

RWTexture2D<float4> resultTexture : register(u0);
Texture2D depthT : register(t0);
Texture2D RenderT : register(t1);


[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    int2 texCoord = threadIndex.xy;
    
    float4 posProj = float4(0,0,0,0);
    
    posProj.xy = (texCoord.xy + 0.5f) / float2(cameraScreenData.x, cameraScreenData.y) * 2.0f - 1.0f;
    posProj.y *= -1;
    posProj.z = depthT[texCoord.xy];
    posProj.w = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    float dist = posView.z / posView.w;
    
    if (g_depthRendering == 1)
    {
        resultTexture[texCoord] = float4(dist * 0.001f, dist * 0.001f, dist * 0.001f, 1.0f);
        return;
    };

    float distFog = saturate((dist - g_fogMin) / (g_fogMax - g_fogMin));
    float fogFactor = exp(-distFog * power);
    dist = length(posView.xyz);
    
    float3 color = lerp(g_fogColor, RenderT[texCoord.xy].xyz, fogFactor);
    
    resultTexture[texCoord] = float4(color, 1.0f);
    
 
}