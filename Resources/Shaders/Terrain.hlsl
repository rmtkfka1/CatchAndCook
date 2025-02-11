#include "GLOBAL.hlsl"
#include  "Light.hlsl"

Texture2D g_tex_0 : register(t0);
Texture2D heightMap : register(t1);
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);
#define TessFactor 8
#define PI 3.14159f
#define DIST_MAX 3000.0f
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

    float4 cameraPos;
    float4 cameraLook;
    float4 cameraUp;
    float4 cameraFrustumData;
    float4 cameraScreenData;
};

cbuffer TerrainDetailsParam : register(b10)
{
	float3 fieldSize;
    int padding;
	int detailsCount;
	float4 tileST[4];
	//Vector4 color;
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
    output.normal = mul(float4(input.normal, 0.0f), WorldMat).xyz;
    output.uv = input.uv;
    
    return output;
}

struct PatchConstOutput
{
    float edges[3] : SV_TessFactor;
    float inside[1] : SV_InsideTessFactor;
};



float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float distance = length(patchPos - cameraWorldPos);
    float ratio = smoothstep(min, max, distance);
    
    float level = lerp(maxLv, 1.0f, ratio);

    return clamp(level, 1.0f, maxLv);
}


//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 3> patch, uint patchID : SV_PrimitiveID)
{

    PatchConstOutput pt;
    
    float3 edge0Pos = (patch[1].pos + patch[2].pos) / 2.f;
    float3 edge1Pos = (patch[2].pos + patch[0].pos) / 2.f;
    float3 edge2Pos = (patch[0].pos + patch[1].pos) / 2.f;

    float edge0TessLevel = CalculateTessLevel(cameraPos.xyz, edge0Pos, DIST_MIN, DIST_MAX, TessFactor);
    float edge1TessLevel = CalculateTessLevel(cameraPos.xyz, edge1Pos, DIST_MIN, DIST_MAX, TessFactor);
    float edge2TessLevel = CalculateTessLevel(cameraPos.xyz, edge2Pos, DIST_MIN, DIST_MAX, TessFactor);

    pt.edges[0] = edge0TessLevel;
    pt.edges[1] = edge1TessLevel;
    pt.edges[2] = edge2TessLevel;
    pt.inside[0] = edge2TessLevel;
    return pt;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64)]
HS_OUT HS_Main(InputPatch<VS_OUT, 3> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    //4번호출됨.
    HS_OUT hout;
    hout.pos = patch[vertexID].pos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    
    return hout;
}



[domain("tri")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 3> quad, PatchConstOutput patchConst, float3 location : SV_DomainLocation)
{
    DS_OUT dout;
    
    float3 pos = quad[0].pos * location.x + quad[1].pos * location.y + quad[2].pos * location.z;
    float2 uv = quad[0].uv * location.x + quad[1].uv * location.y + quad[2].uv * location.z;
    float3 normal = quad[0].normal * location.x + quad[1].normal * location.y + quad[2].normal * location.z;
    
    pos.y += heightMap.SampleLevel(g_sam_1, uv, 0).r * fieldSize.y;
    
    dout.uv = uv;
    dout.pos = mul(float4(pos, 1.0f), VPMatrix);
    dout.normal = normal;
    
    return dout;
}
float4 PS_Main(DS_OUT input) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, input.uv);
}
