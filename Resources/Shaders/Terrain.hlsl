#include "GLOBAL.hlsl"
#include  "Light.hlsl"

Texture2D g_tex_0 : register(t0);
Texture2D heightMap : register(t1);
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);

#define G_MaxTess 4
#define G_MinTess 1
#define PI 3.14159f
#define DIST_MAX 2000.0f
#define DIST_MIN 3.0f


cbuffer test : register(b1)
{
    row_major matrix WorldMat;
}

cbuffer cameraParams : register(b2)
{
    row_major Matrix ViewMatrix;
    row_major Matrix ProjectionMatrix;
    row_major Matrix VPMatrix;
    row_major Matrix InvertViewMatrix;
    row_major Matrix InvertProjectionMatrix;
    row_major Matrix InvertVPMatrix;

    float4 g_cameraPos;
    float4 g_cameraLook;
    float4 g_cameraUp;
    float4 g_cameraFrustumData;
    float4 g_cameraScreenData;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct DS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct HS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(float4(input.pos, 1.0f), WorldMat);
    output.pos.y += heightMap.SampleLevel(g_sam_1, input.uv, 0).r * 1000.0f;
    output.normal = mul(float4(input.normal, 0.0f), WorldMat).xyz;
    output.uv = input.uv;
    
    return output;
}

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

float CalcTessFactor(float3 p)
{
    float d = distance(p, g_cameraPos.xyz);
    float s = smoothstep(DIST_MIN, DIST_MAX, d);
    float tess = exp2(lerp(G_MaxTess, G_MinTess, s));
    
    return clamp(tess, 1.0f, 64.0f); 
}
//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{

    PatchConstOutput pt;
    
    float3 e0 = 0.5f * (patch[0].pos.xyz + patch[2].pos.xyz);
    float3 e1 = 0.5f * (patch[0].pos.xyz + patch[1].pos.xyz);
    float3 e2 = 0.5f * (patch[1].pos.xyz + patch[3].pos.xyz);
    float3 e3 = 0.5f * (patch[2].pos.xyz + patch[3].pos.xyz);
    float3 c = 0.25f * (patch[0].pos.xyz + patch[1].pos.xyz + patch[2].pos.xyz + patch[3].pos.xyz);


    pt.edges[0] = CalcTessFactor(e0);
    pt.edges[1] = CalcTessFactor(e1);
    pt.edges[2] = CalcTessFactor(e2);
    pt.edges[3] = CalcTessFactor(e3);

    pt.inside[0] = CalcTessFactor(c);
    pt.inside[1] = pt.inside[0];
    
    return pt;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    //4번호출됨.
    HS_OUT hout;
    hout.pos = patch[vertexID].pos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    
    return hout;
}



[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 location : SV_DomainLocation)
{
    DS_OUT dout;
    
  
    dout.pos = lerp(
		lerp(quad[0].pos, quad[1].pos, location.x),
		lerp(quad[2].pos, quad[3].pos, location.x),
		location.y);

    dout.uv = lerp(
		lerp(quad[0].uv, quad[1].uv, location.x),
		lerp(quad[2].uv, quad[3].uv, location.x),
		location.y);

    dout.uv = lerp(
		lerp(quad[0].uv, quad[1].uv, location.x),
		lerp(quad[2].uv, quad[3].uv, location.x),
		location.y);
    
    dout.pos = mul(dout.pos, VPMatrix);
    
    return dout;
}
float4 PS_Main(DS_OUT input) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, input.uv);
}
