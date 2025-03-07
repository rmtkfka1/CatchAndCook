#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_b5.hlsl"

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
 
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 worldPos : Position;
    float3 worldNormal : NORMAL;
    float2 uv : TEXCOORD;
    float3 worldTangent : TANGENT;
    
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(float4(input.pos, 1.0f), LocalToWorldMatrix);
    output.worldPos = output.pos.xyz;
    
    float4 clipPos = mul(output.pos, VPMatrix);
    output.pos = clipPos;
    
    output.uv = input.uv;
    
    output.worldNormal = TransformNormalLocalToWorld(input.normal);
    output.worldTangent = TransformNormalLocalToWorld(input.tangent);
    

   
    return output;
}

struct PS_OUT
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 color : SV_Target2;
    float depth : SV_Target3;
};

PS_OUT PS_Main(VS_OUT input) : SV_Target
{
    PS_OUT output = (PS_OUT) 0;
    
    output.position = float4(input.worldPos, 1.0f);
    float3 N= ComputeNormalMapping(input.worldNormal, input.worldTangent, _BumpMap.Sample(sampler_lerp, input.uv));
    output.color = _BaseMap.Sample(sampler_lerp, input.uv); 
    output.depth = input.pos.z;
    return output;
}