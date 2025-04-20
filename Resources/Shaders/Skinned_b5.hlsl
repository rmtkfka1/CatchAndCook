
#ifndef INCLUDE_SKINNED
#define INCLUDE_SKINNED

cbuffer BoneParam : register(b5)
{
    row_major matrix BoneMatrix[256];
    row_major matrix InvertBoneMatrix[256];
}

struct BoneData
{
    row_major matrix BoneMatrix[256];
    row_major matrix InvertBoneMatrix[256];
};


//float4 CalculateBone(float4 localPosition, float4 bIds, float4 bWs)
//{
//    return (mul(localPosition, BoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
//         + mul(localPosition, BoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
//         + mul(localPosition, BoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
//         + mul(localPosition, BoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w))) / (bWs.x + bWs.y + bWs.z + bWs.w);
//}
float4 CalculateBone(float4 lp, float4 bIds, float4 bWs)
{
    // 1) ID → uint 인덱스 (음수는 0으로 클램프)
    uint i0 = (uint)max(bIds.x, 0.0);
    uint i1 = (uint)max(bIds.y, 0.0);
    uint i2 = (uint)max(bIds.z, 0.0);
    uint i3 = (uint)max(bIds.w, 0.0);

    // 2) 가중치 합과 역수 (한 번만)
    float w0 = bWs.x, w1 = bWs.y, w2 = bWs.z, w3 = bWs.w;
    float sumW = w0 + w1 + w2 + w3;
    float invW = 1.0 / sumW;

    // 3) 매트릭스 로드 후 곱하고 가중치 곱셈
    //    mul(row‑vec, row_major matrix) 를 이용
    float4 p = mul(lp, BoneMatrix[i0]) * w0;
           p += mul(lp, BoneMatrix[i1]) * w1;
           p += mul(lp, BoneMatrix[i2]) * w2;
           p += mul(lp, BoneMatrix[i3]) * w3;

    // 4) 정규화
    return p * invW;
}

//float4 CalculateBoneInvert(float4 worldPosition, float4 bIds, float4 bWs)
//{
//    return mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.x)]) * (bWs.x * step(-0.5, bIds.x))
//         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.y)]) * (bWs.y * step(-0.5, bIds.y))
//         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.z)]) * (bWs.z * step(-0.5, bIds.z))
//         + mul(worldPosition, InvertBoneMatrix[max(0, (int)bIds.w)]) * (bWs.w * step(-0.5, bIds.w)) / (bWs.x + bWs.y + bWs.z + bWs.w);
//}

float4 CalculateBoneInvert(float4 wp, float4 bIds, float4 bWs)
{
    uint i0 = (uint)max(bIds.x, 0.0);
    uint i1 = (uint)max(bIds.y, 0.0);
    uint i2 = (uint)max(bIds.z, 0.0);
    uint i3 = (uint)max(bIds.w, 0.0);

    float w0 = bWs.x, w1 = bWs.y, w2 = bWs.z, w3 = bWs.w;
    float sumW = w0 + w1 + w2 + w3;
    float invW = 1.0 / sumW;

    float4 p = mul(wp, InvertBoneMatrix[i0]) * w0;
           p += mul(wp, InvertBoneMatrix[i1]) * w1;
           p += mul(wp, InvertBoneMatrix[i2]) * w2;
           p += mul(wp, InvertBoneMatrix[i3]) * w3;

    return p * invW;
}

//float3 CalculateBoneNormal(float3 localNormal, float4 bIds, float4 bWs)
//{
//    float4 normal = float4(0, 0, 0, 0);

//    // InvertBoneMatrix는 단순 inverse이므로, 노말 변환에 사용하기 위해 transpose를 취한다.
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.x)])) * (bWs.x * step(-0.5, bIds.x));
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.y)])) * (bWs.y * step(-0.5, bIds.y));
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.z)])) * (bWs.z * step(-0.5, bIds.z));
//    normal += mul(float4(localNormal, 0), transpose(InvertBoneMatrix[max(0, (int)bIds.w)])) * (bWs.w * step(-0.5, bIds.w));

//    return normalize(normal.xyz);
//}
float3 CalculateBoneNormal(float3 ln, float4 bIds, float4 bWs)
{
    uint i0 = (uint)max(bIds.x, 0.0);
    uint i1 = (uint)max(bIds.y, 0.0);
    uint i2 = (uint)max(bIds.z, 0.0);
    uint i3 = (uint)max(bIds.w, 0.0);

    float w0 = bWs.x, w1 = bWs.y, w2 = bWs.z, w3 = bWs.w;

    // inverse‑transpose: transpose(InvertBoneMatrix) 의 상위 3×3 부분만 사용
    float3 n = mul(ln, (float3x3)transpose(InvertBoneMatrix[i0])) * w0;
         n += mul(ln, (float3x3)transpose(InvertBoneMatrix[i1])) * w1;
         n += mul(ln, (float3x3)transpose(InvertBoneMatrix[i2])) * w2;
         n += mul(ln, (float3x3)transpose(InvertBoneMatrix[i3])) * w3;

    return normalize(n);
}

#endif