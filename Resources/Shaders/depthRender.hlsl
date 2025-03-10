RWTexture2D<float4> resultTexture : register(u0); 
Texture2D<float> depthTexture : register(t0); 

[numthreads(16, 16, 1)]
void CS_Main(int3 threadIndex : SV_DispatchThreadID)
{
    int2 texCoord = threadIndex.xy;

    float depth = depthTexture.Load(int3(texCoord, 0));
    
    resultTexture[texCoord] = float4(depth, depth, depth, 1.0f);
}