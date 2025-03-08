RWTexture2D<float4> resultTexture : register(u0); // 결과 텍스처
Texture2D<float4> inputTexture : register(t0); // 입력 텍스처


static float Luminance[3] = { 0.2126f, 0.7152f, 0.072f };
static float th = 0.7f;

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{

    int2 texCoord = threadIndex.xy;

    
    float3 color = inputTexture[texCoord];
    
    float relativeLuminance = (color.r * Luminance[0]) + (color.g * Luminance[1]) + (color.b * Luminance[2]);
    
    if (relativeLuminance<th)
    {
        color = float3(0, 0, 0);
    }
    
    // 결과 텍스처에 쓰기
    resultTexture[texCoord] = float4(color, 1.0f);
}