#include "GLOBAL.hlsl"
#include  "Light.hlsl"

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);
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
    float amplitudes[waveCount] = { 11.0f, 9.0f, 7.0f };
    float wavelengths[waveCount] = { 500.0f, 300.0f, 200.0f };
    float speeds[waveCount] = { 0.5f, 1.0f, 0.8f };
    float steepnesses[waveCount] = { 0.5f, 0.4f, 0.3f }; // 스티프니스 추가

    float2 waveDirections[waveCount] =
    {
        normalize(float2(1.0f, 0.2f)),
        normalize(float2(0.3f, -1.0f)),
        normalize(float2(-0.5f, 0.7f))
    };

    float3 modifiedPos = input.pos;
    float dHdX = 0.0f; // x 방향 편미분
    float dHdZ = 0.0f; // z 방향 편미분

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];
        float steepness = steepnesses[i];

        float dotProduct = dot(direction, input.pos.xz);
        float wave = sin(dotProduct * frequency + phase);
        float waveDerivative = cos(dotProduct * frequency + phase);

        // Gerstner Waves 적용
        modifiedPos.x += steepness * amplitudes[i] * direction.x * waveDerivative;
        modifiedPos.z += steepness * amplitudes[i] * direction.y * waveDerivative;
        modifiedPos.y += amplitudes[i] * wave;

        // 편미분 계산
        float dWavedX = frequency * waveDerivative * direction.x;
        float dWavedZ = frequency * waveDerivative * direction.y;

        dHdX += amplitudes[i] * dWavedX;
        dHdZ += amplitudes[i] * dWavedZ;
    }

    // 법선 벡터 계산
    float3 normal = normalize(float3(-dHdX, 1.0f, -dHdZ));

    VS_IN result;
    result.pos = modifiedPos;
    result.normal = normal;

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

    float3x3 normalMatrix = (float3x3) transpose(InvertViewMatrix); // 또는 WorldMat의 역전치 행렬
    output.normal = mul(result.normal, normalMatrix);
    
    return output;
}

struct PatchConstOutput
{
    float edges[3] : SV_TessFactor;
    float inside[1] : SV_InsideTessFactor;
};

//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 3> patch, uint patchID : SV_PrimitiveID)
{
    float3 center = (patch[0].pos + patch[1].pos + patch[2].pos).xyz /3.0f;
    float dist = length(center.xz - cameraPos.xz);
    
    float tess = TessFactor * saturate((DIST_MAX - dist) / (DIST_MAX - DIST_MIN)) + 1.0f; //270

    PatchConstOutput pt;
    pt.edges[0] = tess;
    pt.edges[1] = tess;
    pt.edges[2] = tess;
    pt.inside[0] = tess;

    return pt;
}
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(TessFactor)]
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
   
    dout.pos = mul(float4(pos, 1.0f), VPMatrix);
    dout.uv = uv;
    dout.normal = normal;
    
    return dout;
}

float4 PS_Main(DS_OUT input) : SV_Target
{
    float3 color;
    
    float4 worldPos = mul(input.pos, InvertVPMatrix);
    float3 WolrdNormal = input.normal;
    
    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].mateiral.lightType == 0)
        {
            color += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, WolrdNormal.xyz, -toEye);
        }
        else if (g_lights[i].mateiral.lightType == 1)
        {
            color += ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, -toEye);
        }
        else if (g_lights[i].mateiral.lightType == 2)
        {
            color += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WolrdNormal.xyz, -toEye);
        }
    }
    
    return float4(color, 1.0f) * g_tex_0.Sample(g_sam_0, input.uv);
  
}
