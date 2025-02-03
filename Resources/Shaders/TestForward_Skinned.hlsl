#include "GLOBAL.hlsl"


cbuffer TestTransform : register(b1)
{
    row_major matrix WorldMat;
}

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

cbuffer BoneParam : register(b6)
{
    row_major matrix boneMatrix[256];
}

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 bondIds : BONEIDs;
    float4 boneWs : BONEWs;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
};

Texture2D _BaseMap : register(t0);
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    float4 localPos = float4(input.pos, 1.0f);

    int boneIndex0 = max(0, (int)input.bondIds.x);
    int boneIndex1 = max(0, (int)input.bondIds.y);
    int boneIndex2 = max(0, (int)input.bondIds.z);
    int boneIndex3 = max(0, (int)input.bondIds.w);
    
    // boneIds가 -1이면 유효하지 않으므로 0을, 0 이상이면 1을 반환 (branchless)
    float valid0 = step(-0.5, input.bondIds.x);
    float valid1 = step(-0.5, input.bondIds.y);
    float valid2 = step(-0.5, input.bondIds.z);
    float valid3 = step(-0.5, input.bondIds.w);

    float4 skinnedPos =
          mul(localPos, boneMatrix[boneIndex0]) * (input.boneWs.x * valid0)
        + mul(localPos, boneMatrix[boneIndex1]) * (input.boneWs.y * valid1)
        + mul(localPos, boneMatrix[boneIndex2]) * (input.boneWs.z * valid2)
        + mul(localPos, boneMatrix[boneIndex3]) * (input.boneWs.w * valid3);

    //output.pos = mul(float4(input.pos, 1.0f), WorldMat);
    output.pos = mul(skinnedPos, VPMatrix);

    
    output.uv = input.uv;

    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
   return  _BaseMap.Sample(g_sam_0, input.uv);
}
