
#ifndef INCLUDE_SKINNED
#define INCLUDE_SKINNED

cbuffer BoneParam : register(b5)
{
    row_major matrix BoneMatrix[256];
    row_major matrix InvertBoneMatrix[256];
}

float4 CalculateBone(float4 localPosition, float4 bIds, float4 bWs)
{
    return mul(localPosition, BoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
         + mul(localPosition, BoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
         + mul(localPosition, BoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
         + mul(localPosition, BoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w));
}

float4 CalculateBoneInvert(float4 worldPosition, float4 bIds, float4 bWs)
{
    return mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w));
}

float3 CalculateBoneNormal(float3 localNormal, float4 bIds, float4 bWs)
{
    float4 normal = float4(0, 0, 0, 0);

    // InvertBoneMatrix는 단순 inverse이므로, 노말 변환에 사용하기 위해 transpose를 취한다.
    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.x)])) * (bWs.x * step(-0.5, bIds.x));
    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.y)])) * (bWs.y * step(-0.5, bIds.y));
    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.z)])) * (bWs.z * step(-0.5, bIds.z));
    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.w)])) * (bWs.w * step(-0.5, bIds.w));

    return normalize(normal.xyz);
}

#endif