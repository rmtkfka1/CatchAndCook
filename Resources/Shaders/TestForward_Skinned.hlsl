
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_b5.hlsl"


cbuffer DefaultMaterialParam : register(b7)
{
	float4 color;
	float4 _baseMapST;
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    
    #ifdef SKINNED
    float2 uv1 : TEXCOORD1;
    float2 uv2 : TEXCOORD2;
    float4 color : COLOR;
    float4 boneIds : BONEIDs;
    float4 boneWs : BONEWs;
	#endif
    #ifdef INSTANCED
    MATRIX_DEFINE(instance_trs, 0);
    MATRIX_DEFINE(instance_invert_trs, 1);
    float3 instance_worldPos : FLOAT3_0;
	#endif
};

struct VS_OUT
{
    float4 positionCS : SV_Position;
    float4 positionWS : PositionWS;
    float3 normalOS : NormalOS;
    float3 normalWS : NormalWS;
    float3 tangentOS : TangentOS;
    float3 tangentWS : TangentWS;
    float2 uv : TEXCOORD0;
};

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    row_major float4x4 l2wMatrix = LocalToWorldMatrix;
    row_major float4x4 w2lMatrix = WorldToLocalMatrix;
    float4 boneIds = float4(-1,-1,-1,-1);
    float4 boneWs = 0;

    #ifdef INSTANCED
		l2wMatrix = MATRIX(input.instance_trs);
		w2lMatrix = MATRIX(input.instance_invert_trs);
    #endif
    #ifdef SKINNED
	    boneIds = input.boneIds;
	    boneWs = input.boneWs;
    #endif





    output.normalOS = input.normal;
    output.tangentOS = input.tangent;

	#ifdef INSTANCED
		output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f),  boneIds, boneWs, l2wMatrix);
    #else
        output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f),  boneIds, boneWs);
	#endif
	output.positionCS =  TransformWorldToClip(output.positionWS);

    #ifdef INSTANCED
		output.normalWS = TransformNormalLocalToWorld(output.normalOS, boneIds, boneWs, w2lMatrix);
		output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs, w2lMatrix);
	#else
        output.normalWS = TransformNormalLocalToWorld(input.normal, boneIds, boneWs);
		output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs);
	#endif


    output.uv = input.uv;
    return output;
}
[earlydepthstencil]
float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 totalNormalWS = TransformTangentToSpace(float4(NormalUnpack(_BumpMap.Sample(sampler_lerp, input.uv), 0.2), 0), input.normalWS, input.tangentWS);
    //float3 totalNormalWS = float3(0,1,0);
	return _BaseMap.Sample(sampler_lerp, input.uv) * color * (dot(totalNormalWS, normalize(float3(0,1,-1))) * 0.5 + 0.5);
}
