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


struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float sliceNorm : TEXCOORD1;
    uint sliceIdx : SV_InstanceID;
};

VS_OUT VS_Main(uint vid : SV_VertexID, uint iid : SV_InstanceID)
{
    static const float2 verts[3] = { float2(-1, -1), float2(3, -1), float2(-1, 3) };

    VS_OUT o;
    o.pos = float4(verts[vid], 0, 1);
    o.uv = (verts[vid] + 1) * 0.5;
    o.sliceIdx = iid;
    o.sliceNorm = (iid + 0.5f) / numSlices; 
    return o;
}



float4 PS_Main(VS_OUT input) : SV_Target
{
 
    float ndcZ = input.sliceNorm * 2.0f - 1.0f;
    float4 clipPos = float4(input.uv * 2.0f - 1.0f, ndcZ, 1.0f);
    float4 viewPosH = mul(InvertProjectionMatrix, clipPos);
    viewPosH /= viewPosH.w;
    float3 viewPos = viewPosH.xyz;


    float depth = viewPos.z;

    float density = saturate((waterHeight - depth) / waterHeight); 


    float3 viewDir = normalize(-viewPos);
    float3 lightDirVS = normalize(mul((float3x3) ViewMatrix, lightDir));

    float cosTheta = dot(viewDir, lightDirVS);
    float phase = (1 - phaseG * phaseG) / pow(1 + phaseG * phaseG - 2 * phaseG * cosTheta, 1.5f);
    
    float3 fog = fogColor  * phase * exp(-absorption * depth);

    return float4(fog, 1.0f);
}