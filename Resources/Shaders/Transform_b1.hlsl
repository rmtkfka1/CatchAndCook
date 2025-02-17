
#ifndef INCLUDE_TRANSFORM
#define INCLUDE_TRANSFORM

#include "Camera_b2.hlsl"
#include "Skinned_b5.hlsl"

cbuffer Transform : register(b1)
{
    row_major matrix LocalToWorldMatrix;
	row_major matrix WorldToLocalMatrix;
    float3 worldPosition;
}

float3 NormalUnpack(float3 normalSample, float power)
{
	if (dot(normalSample, normalSample) > 2.999)
		return float3(0,0,1);
	float3 norm = normalSample * 2.0 - 1.0;
	norm.xy *= power;
	return normalize(norm);
}

float4 TransformSpaceToTangent(float4 localPos, float3 normalS, float3 tangentS)
{
    float3 N = normalS;
    float3 T = normalize(tangentS - dot(tangentS, N) * N);
    float3 B = cross(T, N);

	float4x4 localToTangent = float4x4(
         float4(T.x, B.x, N.x, 0),
         float4(T.y, B.y, N.y, 0),
         float4(T.z, B.z, N.z, 0),
         float4(0,   0,   0,   1)
    );
	return mul(localPos, localToTangent);
}

float4 TransformTangentToSpace(float4 tangentPos, float3 normalS, float3 tangentS)
{
    // 정규화된 tangentS 및 normal 계산
    float3 N = normalS;
    float3 T = normalize(tangentS - dot(tangentS, N) * N);
    float3 B = cross(T, N);

    float4x4 tangentToLocal = float4x4(
         float4(T, 0),
         float4(B, 0),
         float4(N, 0),
         float4(0, 0, 0, 1)
    );
    
    return mul(tangentPos, tangentToLocal);
}

float4 TransformLocalToWorld(float4 localPos)
{
	return mul(localPos, LocalToWorldMatrix);
}

float4 TransformLocalToWorld(float4 localPos, float4 boneIds, float4 boneWs)
{
	#ifdef SKINNED
	return CalculateBone(localPos, boneIds, boneWs);
	#endif
	return TransformLocalToWorld(localPos);
}

float3 TransformNormalLocalToWorld(float3 normal)
{
	float4x4 worldIT = transpose(WorldToLocalMatrix);
	return normalize(mul(normal, (float3x3)worldIT));
}

float3 TransformNormalLocalToWorld(float3 normal, float4 boneIds, float4 boneWs)
{
	#ifdef SKINNED
	return CalculateBoneNormal(normal, boneIds, boneWs);
	#endif
	return TransformNormalLocalToWorld(normal);
}


float4 TransformWorldToLocal(float4 worldPos)
{
	return mul(worldPos, WorldToLocalMatrix);
}

float4 TransformWorldToLocal(float4 worldPos, float4 boneIds, float4 boneWs)
{
	#ifdef SKINNED
	return CalculateBoneInvert(worldPos, boneIds, boneWs);
	#endif
	return TransformWorldToLocal(worldPos);
}


// 직접 행렬을 지정하는것 --
float4 TransformLocalToWorld(float4 localPos, float4x4 l2w)
{
	return mul(localPos, l2w);
}

float4 TransformLocalToWorld(float4 localPos, float4 boneIds, float4 boneWs, float4x4 l2w)
{
	#ifdef SKINNED
	return CalculateBone(localPos, boneIds, boneWs);
	#endif
	return TransformLocalToWorld(localPos, l2w);
}

float3 TransformNormalLocalToWorld(float3 normal, float4x4 w2l)
{
	float4x4 worldIT = transpose(w2l);
	return normalize(mul(normal, (float3x3)worldIT));
}

float3 TransformNormalLocalToWorld(float3 normal, float4 boneIds, float4 boneWs, float4x4 w2l)
{
	#ifdef SKINNED
	return CalculateBoneNormal(normal, boneIds, boneWs);
	#endif
	return TransformNormalLocalToWorld(normal, w2l);
}


float4 TransformWorldToLocal(float4 worldPos, float4x4 w2l)
{
	return mul(worldPos, w2l);
}

float4 TransformWorldToLocal(float4 worldPos, float4 boneIds, float4 boneWs, float4x4 w2l)
{
	#ifdef SKINNED
	return CalculateBoneInvert(worldPos, boneIds, boneWs);
	#endif
	return TransformWorldToLocal(worldPos, w2l);
}
//--------------------------------------------------------------------------------------------


float4 TransformWorldToView(float4 worldPos)
{
	return mul(worldPos, ViewMatrix);
}
float4 TransformViewToWorld(float4 viewPos)
{
	return mul(viewPos, InvertViewMatrix);
}
float4 TransformWorldToClip(float4 worldPos)
{
	return mul(worldPos, VPMatrix);
}
float4 TransformClipToWorld(float4 clipPos)
{
	return mul(clipPos, InvertVPMatrix);
}
float4 TransformViewToClip(float4 viewPos)
{
	return mul(viewPos, ProjectionMatrix);
}
float4 TransformClipToView(float4 clipPos)
{
	return mul(clipPos, InvertProjectionMatrix);
}

#endif