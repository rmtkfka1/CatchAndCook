#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"


cbuffer VolumetricData : register(b5)
{
    float3 color; 
    float phaseG; 

    float3 lightDir;
    float waterHeight;

    float2 padding;
    float Absorption; 
    int numSlice; 
};



struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0; 
    float sliceNorm : TEXCOORD1; 
    uint sliceIdx : SV_InstanceID;
};

VS_OUT VS_Main(uint vid : SV_VertexID,
               uint iid : SV_InstanceID)
{
    VS_OUT o;


    static const float2 verts[3] =
    {
        float2(-1, -1), float2(3, -1), float2(-1, 3)
    };
    static const float2 uvs[3] =
    {
        float2(0, 0), float2(1, 0), float2(0, 1)
    };

    o.pos = float4(verts[vid], 0, 1);
    o.uv = uvs[vid];
    o.sliceNorm = iid / (float) (max(numSlice - 1, 1));
    o.sliceIdx = iid;

    return o;
}



float4 PS_Main(VS_OUT IN) : SV_Target
{

    float ndcZ = IN.sliceNorm * 2.0f - 1.0f;
   
    float4 clipPos = float4(IN.uv * 2.0f - 1.0f, ndcZ, 1.0f);


    float4 viewH = mul(InvertProjectionMatrix, clipPos);
    viewH /= viewH.w;
    float3 Pvs = viewH.xyz;
    
    float nearZ = cameraFrustumData.z;
    float farZ = cameraFrustumData.w;
    float sliceStep = (farZ - nearZ) / numSlice;
    float depth = nearZ + sliceStep * IN.sliceIdx;

 
    float density = saturate((waterHeight - depth) / waterHeight);


    float atten = exp(-Absorption * depth);


    float3 lightDirVS = normalize(mul((float3x3) InvertViewMatrix, lightDir));
    float3 viewDir = normalize(-Pvs);
    float cosT = saturate(dot(viewDir, lightDirVS));
    float g2 = phaseG * phaseG;
    float phase = (1 - g2) / (4 * 3.14159265 * pow(1 + g2 - 2 * phaseG * cosT, 1.5));


    float3 scatter = color * phase * density * atten * sliceStep;

    return float4(scatter, 1.0f);
}
