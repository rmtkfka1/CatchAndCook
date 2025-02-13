#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

struct VS_IN
{
    float3 localPos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD;
};

TextureCube g_tex_0 : register(t0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 로컬 좌표를 그대로 전달
    output.localPos = input.localPos;
    // Translation은 하지 않고 Rotation만 적용한다
    float4 clipSpacePos = mul(float4(input.localPos, 0), VPMatrix);
    output.pos = clipSpacePos.xyww;
    

    return output;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    return g_tex_0.Sample(sampler_lerp, input.localPos);  
}