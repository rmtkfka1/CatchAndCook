#include "GLOBAL.hlsl"
#include  "Light.hlsl"

Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

#define TessFactor 8
#define PI 3.14159f
#define DIST_MAX 300.0f
#define DIST_MIN 30.0f

static float4 sea_color = float4(0.3f,0.3f,1.0f,1.0f);

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

VS_IN WaveGeneration(VS_IN input)
{
    const int waveCount = 3;
    float amplitudes[waveCount] = {11.0f, 9.0f, 7.0f };
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f };
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f };

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)),
        normalize(float2(0.0f, 1.0f)),
        normalize(float2(-0.5f, 0.7f))
    };

    float3 modifiedPos = input.pos;
    float3 normalSum = float3(0.0f, 0.0f, 0.0f); // 초기 법선 (0으로 초기화)

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase) * frequency;

        // 높이 변위 적용
        modifiedPos.y += amplitudes[i] * wave;

        // 법선 기울기 계산
        float3 tangent = float3(1.0f, waveDerivative * direction.x * amplitudes[i], 0.0f);
        float3 binormal = float3(0.0f, waveDerivative * direction.y * amplitudes[i], 1.0f);

        float3 normal = normalize(cross(tangent, binormal));
        normalSum += normal;
    }

    // 최종 정규화
    normalSum = normalize(normalSum);

    VS_IN result;
    result.pos = modifiedPos;
    result.normal = normalSum;
    result.uv = input.uv; // UV 유지

    return result;
}

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    VS_IN result = WaveGeneration(input);
 
    // 월드, 뷰, 프로젝션 변환
    float4 worldPos = mul(float4(result.pos.xyz, 1.0f), WorldMat);
    output.pos = worldPos;
    output.uv = input.uv;
    output.normal = input.normal;
    return output;
}

struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    float3 center = (patch[0].pos + patch[1].pos + patch[2].pos + patch[3].pos).xyz * 0.25f;
    float dist = length(center.xz - cameraPos.xz);
    
    float tess = TessFactor * saturate((DIST_MAX - dist) / (DIST_MAX - DIST_MIN)) + 1.0f; //270

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
    hout.normal = patch[vertexID].normal;
    
    return hout;
}


[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 location : SV_DomainLocation)
{
    DS_OUT dout;
    
    float4 v1 = lerp(quad[0].pos, quad[1].pos, location.x);
    float4 v2 = lerp(quad[2].pos, quad[3].pos, location.x);
    float4 posCoord = lerp(v1, v2, location.y);
    dout.pos = mul(posCoord, VPMatrix);

    float2 v3 = lerp(quad[0].uv, quad[1].uv, location.x);
    float2 v4 = lerp(quad[2].uv, quad[3].uv, location.x);
    float2 uvCoord = lerp(v3, v4, location.y);
    dout.uv = uvCoord;

    float3 v5 = lerp(quad[0].normal, quad[1].normal, location.x);
    float3 v6 = lerp(quad[2].normal, quad[3].normal, location.x);
    float3 normal = normalize(lerp(v5, v6, location.y)); // 보간 후 정규화
    dout.normal = mul(float4(normal, 0.0f), WorldMat);
    
    return dout;
}

float4 PS_Main(DS_OUT input) : SV_Target
{
    
    float3 color;
    
    float4 worldPos = mul(input.pos, InvertVPMatrix);
    float3 WolrdNormal = input.normal;
    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);
    
    return g_tex_0.Sample(g_sam_0, input.uv);
  
}
