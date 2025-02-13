
#ifndef INCLUDE_SKINNED
#define INCLUDE_SKINNED

cbuffer BoneParam : register(b6)
{
    row_major matrix BoneMatrix[256];
}

float4 CalculateBone(float4 localPosition, float4 bIds, float4 bWs)
{
    return mul(localPosition, BoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
         + mul(localPosition, BoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
         + mul(localPosition, BoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
         + mul(localPosition, BoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w));
}

#endif