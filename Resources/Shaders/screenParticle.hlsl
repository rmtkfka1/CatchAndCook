#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

struct VS_OUT 
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
    float life : PSIZE0;
    float radius : PSIZE1;
};

struct GS_OUT
{
    float4 pos : SV_POSITION; 
    float2 texCoord : TEXCOORD;
    float3 color : COLOR;
    uint primID : SV_PrimitiveID;
};

struct Particle
{
    float3 pos;
    float3 velocity;
    float3 color;
    float life;
    float radius;
};

StructuredBuffer<Particle> particles : register(t0);

VS_OUT VS_Main(uint vertexID : SV_VertexID)
{
    const float fadeLife = 0.2f;
    
    Particle p = particles[vertexID];
    
    VS_OUT output;
    
    output.pos = float4(p.pos.xyz, 1.0);
    output.color = p.color * saturate(p.life / fadeLife);
    output.life = p.life;
    output.radius = p.radius;
     
    return output;
}


[maxvertexcount(4)]
void GS_Main(point VS_OUT input[1], uint primID : SV_PrimitiveID, inout TriangleStream<GS_OUT> outputStream)
{
    if (input[0].life < 0.0f)
        return;
     
    float hw = input[0].radius; // halfWidth
    float3 up = float3(0, 1, 0);
    float3 right = float3(1, 0, 0);

    GS_OUT output;
    
    output.pos.w = 1;
    output.color = input[0].color;
    
    output.pos.xyz = input[0].pos.xyz - hw * right - hw * up;
    output.texCoord = float2(0.0, 1.0);
    output.primID = primID;
    
    outputStream.Append(output);

    output.pos.xyz = input[0].pos.xyz - hw * right + hw * up;
    output.texCoord = float2(0.0, 0.0);
    output.primID = primID;
    
    outputStream.Append(output);
    
    output.pos.xyz = input[0].pos.xyz + hw * right - hw * up;
    output.texCoord = float2(1.0, 1.0);
    output.primID = primID;

    outputStream.Append(output);
    
    output.pos.xyz = input[0].pos.xyz + hw * right + hw * up;
    output.texCoord = float2(1.0, 0.0);
    output.primID = primID;

    outputStream.Append(output);

    outputStream.RestartStrip(); 
}

float smootherstep(float x, float edge0 = 0.0f, float edge1 = 1.0f)
{
    x = clamp((x - edge0) / (edge1 - edge0), 0, 1);

    return x * x * x * (3 * x * (2 * x - 5) + 10.0f);
}


float4 PS_Main(GS_OUT input) : SV_TARGET
{
    float dist = length(float2(0.5, 0.5) - input.texCoord) * 2;
    float scale = smootherstep(1 - dist);
    return float4(input.color.rgb * scale, 1);
}
