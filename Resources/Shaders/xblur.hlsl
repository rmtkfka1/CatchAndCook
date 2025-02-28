#include "Global_b0.hlsl"

RWTexture2D<float4> resultTexture : register(u0); // 결과 텍스처
Texture2D<float4> inputTexture : register(t0); // 입력 텍스처

static const float weights[11] =
{
    0.05f, 0.05f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.05f, 0.05f,
};

//static const float weights[5] =
//{
//    0.0545f, 0.2442f, 0.4026f, 0.2442f, 0.0545f
//};

static const int blurRadius = 5;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    //if (texCoord.x < 0 || texCoord.x >= g_window_size.x || texCoord.y < 0 || texCoord.y >= g_window_size.y)
    //    return;

    float3 blurColor = float3(0, 0, 0);

    for (int i = -blurRadius; i <= blurRadius; i++)
    {
       
        int2 sampleCoord = texCoord + int2(i, 0);
        
        sampleCoord.x = clamp(sampleCoord.x, 0, g_window_size.x - 1);
        sampleCoord.y = clamp(sampleCoord.y, 0, g_window_size.y - 1);
         
        float3 color = inputTexture.Load(int3(sampleCoord, 0)).rgb;
        
        blurColor += weights[i + blurRadius] * color;
  
    };

    // 결과 텍스처에 쓰기
    resultTexture[texCoord] = float4(blurColor, 1.0f);
    
}