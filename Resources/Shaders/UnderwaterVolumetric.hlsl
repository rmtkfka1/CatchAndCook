
#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"

cbuffer VolumetricData:register(b5)
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
    float sliceNorm : TEXCOORD0;
};

VS_OUT VS_Main(uint vid : SV_VertexID, uint iid : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    float2 verts[3] = { float2(-1, -1), float2(3, -1), float2(-1, 3) };

    VS_OUT o;
    o.pos = float4(verts[vid], 0, 1);

    o.sliceNorm = iid / float(numSlice - 1);
    return o;
}


float4 PS_Main(VS_OUT IN) : SV_Target
{
    // 1) [0,1] ¡æ NDC Z
    float ndcZ = IN.sliceNorm * 2 - 1;

    // 2) Å¬¸³¡æºä º¹¿ø
    float2 uv = IN.pos.xy * 0.5 + 0.5;
    float4 clip = float4(uv * 2 - 1, ndcZ, 1);
    float4 viewH = mul(InvertProjectionMatrix, clip);
    viewH /= viewH.w;
    float3 Pvs = viewH.xyz;

   
    float4 worldH = mul(InvertViewMatrix, float4(Pvs, 1));
    worldH /= worldH.w;
    float3 Pws = worldH.xyz;

    float density = saturate((waterHeight - Pws.y) / waterHeight);

    float cosT = saturate(dot(normalize(Pvs), lightDir));
    float g2 = phaseG * phaseG;
    float phase = (1 - g2) / (4 * 3.14159 * pow(1 + g2 - 2 * phaseG * cosT, 1.5));

    float cameraNear = cameraFrustumData.z;
    float cameraFar = cameraFrustumData.w;
    
    float viewDist = IN.sliceNorm * (cameraFar - cameraNear);
    float atten = exp(-Absorption * viewDist);

    float sliceStep = (cameraFar - cameraNear) / numSlice;
    float3 scatter = color * phase * density * atten * sliceStep;

    return float4(scatter, 0);
}