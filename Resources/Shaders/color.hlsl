#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"


struct Instance_Transform
{
    row_major Matrix localToWorld;
    row_major Matrix worldToLocal;
    //float3 worldPosition;
};


StructuredBuffer<Instance_Transform> instanceDatas : register(t5);


struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float4 color : COLOR;
};

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform data = instanceDatas[id];
    
    output.pos = mul(float4(input.pos, 1.0f), data.localToWorld);
    output.pos = mul(output.pos, VPMatrix);

    output.color = input.color;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    return input.color;
}
