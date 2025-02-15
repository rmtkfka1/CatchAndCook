
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
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
    float4 boneIds : BONEIDs;
    float4 boneWs : BONEWs;
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
    float3 test : Position0;
};

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

	float4 boneIds = 0;
    float4 boneWs = 0;
    #ifdef SKINNED
    boneIds = input.boneIds;
    boneWs = input.boneWs;
    #endif

    output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs);
    output.positionCS =  TransformWorldToClip(output.positionWS);
    output.normalOS = input.normal;
    output.normalWS = TransformNormalLocalToWorld(output.normalOS, boneIds, boneWs);
    output.tangentOS = input.tangent;
    output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs);

    output.uv = input.uv;
    output.test = TransformWorldToLocal(float4(normalize(float3(0,1,-1)), 0)).xyz;
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 normalMapwS = TransformTangentToSpace(float4(NormalUnpack(_BumpMap.Sample(sampler_lerp, input.uv), 0.2f), 0), input.normalWS, input.tangentWS);
	return _BaseMap.Sample(sampler_lerp, input.uv) * color * dot(normalMapwS, normalize(float3(0,1,-1)));
}
