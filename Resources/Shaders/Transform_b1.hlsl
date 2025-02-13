
#ifndef INCLUDE_TRANSFORM
#define INCLUDE_TRANSFORM

#define SKINNED

#include "Camera_b2.hlsl"
#include "Skinned_b6.hlsl"

cbuffer Transform : register(b1)
{
    row_major matrix LocalToWorldMatrix;
	row_major matrix WorldToLocalMatrix;
    float3 worldPosition;
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
//output.pos = mul(output.pos, VPMatrix);