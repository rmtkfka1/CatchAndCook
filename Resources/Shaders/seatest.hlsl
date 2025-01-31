#include "GLOBAL.hlsl"
Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

#define TessFactor 64
#define PI 3.14159f
#define DIST_MAX 250.0f
#define DIST_MIN 100.0f

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

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : POSITION;
    float4 worldPos : POSITION1;
    float2 uv : TEXCOORD;
};

struct DS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};


DS_OUT WaveGeneration(DS_OUT input)
{
    const int waveCount = 3;
    float amplitudes[waveCount] = { 9.0f, 6.0f, 4.0f };
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f };
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f };

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)),
        normalize(float2(0.0f, 1.0f)),
        normalize(float2(-0.5f, 0.7f))
    };

    float4 modifiedPos = input.pos;
    float3 normalSum = float3(0.0f, 1.0f, 0.0f);

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, modifiedPos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

        modifiedPos.xz += amplitudes[i] * direction * waveDerivative;
        modifiedPos.y += amplitudes[i] * wave;

        float3 waveNormal = normalize(float3(-waveDerivative * direction.x, 1.0f, -waveDerivative * direction.y));
        normalSum += waveNormal;
    }

    normalSum = normalize(normalSum);

    DS_OUT result;
    result.pos = modifiedPos;
    result.uv = input.uv;
    return result;
    
    
    
}

VS_OUT VS_Main(VS_IN vin)
{
    VS_OUT vout;
    
    vout.pos = mul(float4(vin.pos, 1.0f), WorldMat);
    vout.worldPos = vout.pos;
    vout.uv = vin.uv;
    
    return vout;
}

struct HS_OUT
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};


//패치단위로 호출됨.
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    float3 center = (patch[0].worldPos + patch[1].worldPos + patch[2].worldPos + patch[3].worldPos).xyz * 0.25f;
    float dist = length(center.xz - cameraPos.xz);

    float tessMin = 2.0f;
    float tessMax = 64.0f;

    float t = saturate((dist - DIST_MIN) / (DIST_MAX - DIST_MIN));
    float tess = lerp(tessMax, tessMin, t);

    PatchConstOutput pt;
    pt.edges[0] = tess;
    pt.edges[1] = tess;
    pt.edges[2] = tess;
    pt.edges[3] = tess;
    pt.inside[0] = tess;
    pt.inside[1] = tess;

    return pt;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(TessFactor)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    //4번호출됨.
    HS_OUT hout;
    hout.pos = patch[vertexID].pos;
    hout.uv = patch[vertexID].uv;
    
    return hout;
}


[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 location : SV_DomainLocation)
{
    //쪼개진 정점갯수만큼 호출됨.
    DS_OUT dout;

	// Bilinear interpolation.
    float4 v1 = lerp(quad[0].pos, quad[1].pos, location.x);
    float4 v2 = lerp(quad[2].pos, quad[3].pos, location.x);
    float4 posCoord = lerp(v1, v2, location.y);
    
    dout.pos = posCoord;

    float2 v3 = lerp(quad[0].uv, quad[1].uv, location.x);
    float2 v4 = lerp(quad[2].uv, quad[3].uv, location.x);
    float2 uvCoord = lerp(v3, v4, location.y);
    
    dout.uv = uvCoord;
    
    DS_OUT result = WaveGeneration(dout);
    
    result.pos = mul(result.pos, VPMatrix);

    return result;
}

float4 PS_Main(DS_OUT input) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, input.uv);
}
