#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"


Texture2D _baseMap : register(t0);
Texture2DArray _bumpMap : register(t1);
TextureCube _cubeMap : register(t2);


#define G_MaxTess 4
#define G_MinTess 1

#define PI 3.14159f
#define DIST_MAX 900.0f
#define DIST_MIN 10.0f



///////////////////////////////////////////////////////////////////////////
// 1) 정점 셰이더 관련 구조체
///////////////////////////////////////////////////////////////////////////

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

///////////////////////////////////////////////////////////////////////////
// 2) Hull/Domain 셰이더에 전달될 구조체
///////////////////////////////////////////////////////////////////////////

struct HS_OUT
{
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct DS_OUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

///////////////////////////////////////////////////////////////////////////
// 3) Gerstner Wave 함수 
///////////////////////////////////////////////////////////////////////////
void WaveGeneration(inout float3 worldPos, inout float3 worldNormal)
{
    const int waveCount = 3;

    // 여러 파도 파라미터
    float amplitudes[waveCount] = { 2.5f, 1.5f, 1.0f };
    float wavelengths[waveCount] = { 150.f, 200.f, 150.f };
    float speeds[waveCount] = { 1.0f, 0.4f, 3.0f };
    float steepnesses[waveCount] = { 0.5f, 0.4f, 0.3f };

    float2 waveDirections[waveCount] =
    {
        normalize(float2(0.4f, 0.2f)),
        normalize(float2(0.7f, -1.0f)),
        normalize(float2(0.5f, 0.7f))
    };

    float3 modifiedPos = worldPos;

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2.0f * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 dir = waveDirections[i];
        float steep = steepnesses[i];

        // 현재 정점(worldPos)과 파도 방향(dir)의 내적
        float dotVal = dot(dir, worldPos.xz);

        // sin / cos 로 파도 변위 계산
        float waveSin = sin(dotVal * frequency + phase);
        float waveCos = cos(dotVal * frequency + phase);

        // Gerstner 공식에 의한 xz 변위
        modifiedPos.x += steep * amplitudes[i] * dir.x * waveCos;
        modifiedPos.z += steep * amplitudes[i] * dir.y * waveCos;
        // y 변위
        modifiedPos.y += amplitudes[i] * waveSin;
    }

    // 결과 반영
    worldPos = modifiedPos;
}


///////////////////////////////////////////////////////////////////////////
// 4) Vertex Shader
///////////////////////////////////////////////////////////////////////////
VS_OUT VS_Main(VS_IN input, uint instanceID : SV_InstanceID)
{
    VS_OUT output;

    // 인스턴스 변환 행렬 가져오기 (프로젝트 환경에 맞춰 사용)
    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + instanceID];
    row_major float4x4 l2wMatrix = data.localToWorld;

    // 정점 -> 월드
    float4 worldPos = mul(float4(input.pos, 1.0f), l2wMatrix);
    float3 worldNormal = mul(float4(input.normal, 0.0f), l2wMatrix);

    output.worldPos = worldPos;
    output.uv = input.uv;
    output.normal = input.normal;
    return output;
}

///////////////////////////////////////////////////////////////////////////
// 5) Hull Shader (테셀레이션 제어를 위한 상수 셰이더)
///////////////////////////////////////////////////////////////////////////
struct PatchConstOutput
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

float CalcTessFactor(float3 p)
{
    
    float d = distance(p, cameraPos.xyz);
    float s = smoothstep(DIST_MIN, DIST_MAX, d);
    float tess = exp2(lerp(G_MaxTess, G_MinTess, s));
    return clamp(tess, 1.0f, 64.0f);
}


PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchConstOutput pc;
    
  
    float3 e0 = 0.5f * (patch[0].worldPos.xyz + patch[2].worldPos.xyz);
    float3 e1 = 0.5f * (patch[0].worldPos.xyz + patch[1].worldPos.xyz);
    float3 e2 = 0.5f * (patch[1].worldPos.xyz + patch[3].worldPos.xyz);
    float3 e3 = 0.5f * (patch[2].worldPos.xyz + patch[3].worldPos.xyz);
    float3 c = 0.25f * (patch[0].worldPos.xyz + patch[1].worldPos.xyz + patch[2].worldPos.xyz + patch[3].worldPos.xyz);

    pc.edges[0] = CalcTessFactor(e0);
    pc.edges[1] = CalcTessFactor(e1);
    pc.edges[2] = CalcTessFactor(e2);
    pc.edges[3] = CalcTessFactor(e3);

    pc.inside[0] = CalcTessFactor(c);
    pc.inside[1] = pc.inside[0];
    
    return pc;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HS_OUT HS_Main(InputPatch<VS_OUT, 4> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HS_OUT hout;
    hout.worldPos = patch[vertexID].worldPos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    return hout;
}

///////////////////////////////////////////////////////////////////////////
// 6) Domain Shader
///////////////////////////////////////////////////////////////////////////
[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 uvDomain : SV_DomainLocation)
{
    DS_OUT dout;
    
    float4 p0 = lerp(quad[0].worldPos, quad[1].worldPos, uvDomain.x);
    float4 p1 = lerp(quad[2].worldPos, quad[3].worldPos, uvDomain.x);
    dout.worldPos = lerp(p0, p1, uvDomain.y);

    float2 t0 = lerp(quad[0].uv, quad[1].uv, uvDomain.x);
    float2 t1 = lerp(quad[2].uv, quad[3].uv, uvDomain.x);
    dout.uv = lerp(t0, t1, uvDomain.y);

    float3 n0 = lerp(quad[0].normal, quad[1].normal, uvDomain.x);
    float3 n1 = lerp(quad[2].normal, quad[3].normal, uvDomain.x);
    dout.normal = lerp(n0, n1, uvDomain.y);

    WaveGeneration(dout.worldPos.xyz, dout.normal.xyz);

   
    dout.clipPos = mul(dout.worldPos, VPMatrix);
    return dout;
}

float3 GetSkyColor(float3 dir, float3 c)
{
    dir.y = max(0.0, dir.y);
    float et = 1.0 - dir.y;
    return (1.0 - c) * et + c;
}
///////////////////////////////////////////////////////////////////////////
// 7) Pixel Shader
///////////////////////////////////////////////////////////////////////////
float4 PS_Main(DS_OUT input) : SV_Target0
{  
    
    ComputeNormalMapping(input.normal, float3(1, 0, 0), _bumpMap.Sample(sampler_aniso16, float3(input.uv * 4, 0)));
    

    float3 viewDir = normalize(g_eyeWorld - input.worldPos.xyz);
    float3 reflectDir = normalize(reflect(-viewDir, normalize(input.normal)));
    float3 sea_reflect_color = GetSkyColor(reflectDir, normalize(float3(0, 104, 255)));
    
    return float4(sea_reflect_color, 1.0f);
}
