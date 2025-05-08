

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


RWTexture2D<float4> resultTexture : register(u0);
Texture2D<float4> RenderTexture : register(t1);
Texture2D DepthTexture : register(t2);


float3 ProjToView(int2 texCoord)
{
    float2 uv = (float2(texCoord) + 0.5) / float2(cameraScreenData.x, cameraScreenData.y);
    float depth = DepthTexture.Load(int3(texCoord, 0));
    float4 ndc = float4(uv.x * 2.0 - 1.0, 1.0 - uv.y * 2.0, depth, 1.0);
    float4 viewPos = mul(InvertProjectionMatrix, ndc);
    return viewPos.xyz / viewPos.w;
}



cbuffer data : register(b3)
{
    float3 WATER_ABSORPTION;
    float p1;
    
    float3 WATER_SCATTER;
    float DENSITY;
};


float3 ApplyMediumScattering(float3 color,float depth,float3 lightDir,float3 viewDir,float3 worldPos)
{
 
    float distanceInMedium = depth * DENSITY;

    float3 absorption = exp(-WATER_ABSORPTION * distanceInMedium);

    float cosTheta = dot(normalize(-viewDir), normalize(lightDir));
    float g = 0.8; 
    float phase = (1 - g * g) / (4 * 3.1415 * pow(1 + g * g - 2 * g * cosTheta, 1.5));
    
    float3 scatterLight = WATER_SCATTER * phase * distanceInMedium;

    return color * absorption + scatterLight;
}


[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    float3 color = RenderTexture[texCoord.xy].xyz;
    float3 viewPos = ProjToView(texCoord);
    float depth = length(viewPos); 

    float3 worldPos = cameraPos.xyz + normalize(viewPos) * depth;

    // 매질 효과 적용
    float3 lightDir = normalize(float3(0, -1, -1)); 
    color = ApplyMediumScattering(color, depth, lightDir, normalize(viewPos), worldPos);
    
    resultTexture[texCoord] = float4(color, 1.0);
}