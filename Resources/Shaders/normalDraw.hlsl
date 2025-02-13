#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"


struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 normal : NORMAL;
};

struct GS_OUT
{ 
    float4 pos : SV_Position;
    float4 color : COLOR;
};


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.0f);
    output.normal = input.normal;

    return output;
}

[maxvertexcount(2)]
void GS_Main(point VS_OUT input[1], inout LineStream<GS_OUT> outputStream)
{
    
    GS_OUT output;
    
    float4 worldPos = mul(float4(input[0].pos.xyz, 1.0f), LocalToWorldMatrix);
    float4 normalWorld = mul(float4(input[0].normal, 0.0f), LocalToWorldMatrix);
    
    normalWorld = normalize(normalWorld);
    output.pos = mul(worldPos, VPMatrix);
    output.color = float4(1, 0, 0, 0);
    outputStream.Append(output);
    
    output.pos = mul(worldPos + normalWorld * 1.0f, VPMatrix);
    output.color = float4(0, 1, 0, 0);
    outputStream.Append(output);
    
}


float4 PS_Main(GS_OUT input) : SV_Target
{
    return input.color;
}
