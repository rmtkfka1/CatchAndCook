#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"


cbuffer VolumetricData : register(b5)
{
    float3 fogColor; 
    float phaseG; 
    float3 lightDir;
    float waterHeight; 
    float absorption; 
    int numSlices;
};


struct VS_IN
{
    float4 Position : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float sliceNorm : TEXCOORD1;
    uint sliceIdx : SV_InstanceID;
};

VS_OUT VS_Main(VS_IN input, uint iid : SV_InstanceID)
{
    VS_OUT o;

    o.pos = float4(input.Position.xy, 0.0f, 1.0f);
    o.uv = input.uv;
    o.sliceIdx = iid;
    o.sliceNorm = (iid+0.5f) / numSlices;

    return o;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

    float ndcZ = input.sliceNorm * 2.0f - 1.0f;
    float4 clipPos = float4(input.uv * 2.0f - 1.0f, ndcZ, 1.0f);
    clipPos.y *= -1;
    
    float4 viewPosH = mul(InvertProjectionMatrix, clipPos);
    viewPosH /= viewPosH.w;
    float3 viewPos = viewPosH.xyz;

    float depth = viewPos.z;
    
    if(depth>500.0f)
    {
        return float4(0, 0, 0, 0);
    }
   
    float density = saturate(depth / waterHeight);

    float3 viewDir = normalize(viewPos);

    float3 lightDirVS = normalize(mul((float3x3) ViewMatrix, lightDir));
 
    float cosTheta = dot(viewDir, lightDirVS);
    float phase = (1 - phaseG * phaseG) /
                  pow(1 + phaseG * phaseG - 2 * phaseG * cosTheta, 1.5f);


    float3 fog = fogColor  * phase * exp(-absorption * depth);

    return float4(fog, 1.0f); 
}