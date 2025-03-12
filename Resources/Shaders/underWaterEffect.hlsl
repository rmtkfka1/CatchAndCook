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

cbuffer underWatgerParam : register(b1)
{
    float3 g_fogColor;
    float g_fog_power;

    float3 g_underWaterColor;
    float g_fogMin; // 기본 0

    float2 padding;
    int g_on; // (예: -1: 비활성화, 1: 활성화)
    float g_fogMax; // 기본 1000.0f 정도
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
Texture2D<float4> PositionT : register(t2);
Texture2D<float4> ColorGrading : register(t3);


float3 ProjToView(float2 texCoord)
{
    float4 posProj;
    // texCoord는 픽셀 좌표이므로 정규화 좌표로 변환
    posProj.xy = (texCoord + 0.5f) / cameraScreenData.xy * 2.0f - 1.0f;
    posProj.y *= -1; 
    posProj.z = depthT.Load(int3(texCoord, 0));
    posProj.w = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    return posView.xyz / posView.w;
}


float CalculateFogFactor(float3 posView)
{
    float dist = posView.z;
    float distFog = saturate((dist - g_fogMin) / (g_fogMax - g_fogMin));
    float fogFactor = exp(-distFog * g_fog_power);
    return fogFactor;
}

///////////////////////////////////////////////////////////////////////////////
[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    float2 uv = (float2(texCoord) + 0.5f) / cameraScreenData.xy;
    
    float2 distortion = float2(sin(uv.y * 10.0f + g_Time), cos(uv.x * 10.0f + g_Time)) * 0.06f;
    float2 uvDistorted = uv + distortion *0.02f;
    
    float3 BaseColor = RenderT.SampleLevel(sampler_lerp, uvDistorted, 0).xyz;
    BaseColor *= g_underWaterColor;

    float3 viewPos = ProjToView(float2(texCoord));
    float fogFactor = CalculateFogFactor(viewPos);
    
    float3 finalColor = lerp(g_fogColor, BaseColor, fogFactor);
   
    resultTexture[texCoord] = float4(finalColor, 1.0f);
}
