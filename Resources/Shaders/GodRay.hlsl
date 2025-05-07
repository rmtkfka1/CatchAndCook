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

cbuffer RayData : register(b1)
{
    float4 lightWorldPos;       // 월드 공간의 광원 위치 (dir light 는 far 포인트)
    float2 lightScreenUV;       // 미리 계산해 전달: WorldPos → NDC → UV
    int   sampleCount;          // e.g. 50
    float decay;                // e.g. 0.95
    float exposure;             // e.g. 0.3

    float2 padding_ray;
};

float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}

RWTexture2D<float4> resultTexture : register(u0);

Texture2D depthT : register(t0); 
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> EmmT : register(t2);
// RayData 

[numthreads(16,16,1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{

    static const int count = 50;
    int2 texCoord = dispatchThreadID.xy;

    float2 uv = (float2(texCoord) + 0.5) / g_window_size;

    float2 lightUV = lightScreenUV;



    float2 dir = uv - lightUV;
    dir /= length(dir);
    float2 step = dir / count;
    step *= 0.125;

    // 4) 누적 변수
    float illuminationDecay = 10.0;
    float weight           = 1.0 / count;
    float3 godRayColor     = 0;

    float2 currentUV = uv;
    [unroll]
    for (int i = 0; i < count; ++i)
    {
        currentUV -= step;  // 광원 방향으로 이동
        // OcclusionMap: 앞서 만든 밝기 맵
        float3 occ = EmmT.SampleLevel(sampler_lerp, currentUV, 0);
        godRayColor += occ * illuminationDecay * weight;
        illuminationDecay *= decay;
    }

    // 5) 노출 보정
    godRayColor *= exposure;

    // 0미만→0, 5이상→1, 그 사이 선형 스무스
    float fade     = smoothstep(0.0, 5.0, NdcDepthToViewDepth(depthT.SampleLevel(sampler_lerp, uv, 0).r));

    // 6) 원본 장면과 합성
    float3 sceneCol = RenderT.SampleLevel(sampler_lerp, uv, 0).rgb;
    resultTexture[texCoord] = float4(sceneCol + godRayColor * fade, 1);
    //resultTexture[texCoord] = EmmT.SampleLevel(sampler_point, uv, 0);
    //resultTexture[texCoord] = float4(godRayColor, 1);
    //resultTexture[texCoord] = length(dir);
    //resultTexture[texCoord] = float4(sceneCol, 1);
}