#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

#include  "Light_b3.hlsl"

Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);

#define G_MaxTess 4
#define G_MinTess 1

#define PI 3.14159f
#define DIST_MAX 900.0f
#define DIST_MIN 10.0f

static float4 sea_color = float4(0.3f, 0.3f, 1.0f, 1.0f);


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

struct DS_OUT
{
    float4 clipPos : SV_POSITION;
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct HS_OUT
{
    float4 worldPos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

DS_OUT WaveGeneration(DS_OUT input)
{
    const int waveCount = 3;
    float amplitudes[waveCount] = { 2.5f, 1.5f, 1.0f };
    float wavelengths[waveCount] = { 150.0f, 200.0f, 150.0f };
    float speeds[waveCount] = { 1.0f, 0.4f, 3.0f };
    float steepnesses[waveCount] = { 0.5f, 0.4f, 0.3f };

    float2 waveDirections[waveCount] =
    {
        normalize(float2(0.4f, 0.2f)),
        normalize(float2(0.7f, -1.0f)),
        normalize(float2(0.5f, 0.7f))
    };

    float3 modifiedPos = input.worldPos.xyz;
    float dHdX = 0.0f;
    float dHdZ = 0.0f;

    for (int i = 0; i < waveCount; i++)
    {
        float frequency = 2 * PI / wavelengths[i];
        float phase = speeds[i] * g_Time;
        float2 direction = waveDirections[i];
        float steepness = steepnesses[i];

        float dotProduct = dot(direction, input.worldPos.xz);
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

    input.worldPos = float4(modifiedPos, 1.0f);
    input.normal = normal;

    return input;
}

VS_OUT VS_Main(VS_IN input , uint id :SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;
    
    Instance_Transform data = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    row_major float4x4 l2wMatrix = data.localToWorld;
    row_major float4x4 w2lMatrix = data.worldToLocal;
    // 월드, 뷰, 프로젝션 변환
    float4 worldPos = mul(float4(input.pos.xyz, 1.0f), l2wMatrix);
    output.worldPos = worldPos;
    output.uv = input.uv;
    output.normal = mul(float4(input.normal, 0.0f), l2wMatrix);
    
    return output;
}

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

//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchConstOutput pt;
    
    float3 e0 = 0.5f * (patch[0].worldPos.xyz + patch[2].worldPos.xyz);
    float3 e1 = 0.5f * (patch[0].worldPos.xyz + patch[1].worldPos.xyz);
    float3 e2 = 0.5f * (patch[1].worldPos.xyz + patch[3].worldPos.xyz);
    float3 e3 = 0.5f * (patch[2].worldPos.xyz + patch[3].worldPos.xyz);
    float3 c = 0.25f * (patch[0].worldPos.xyz + patch[1].worldPos.xyz + patch[2].worldPos.xyz + patch[3].worldPos.xyz);


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
    hout.worldPos = patch[vertexID].worldPos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    
    return hout;
}


[domain("quad")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 4> quad, PatchConstOutput patchConst, float2 location : SV_DomainLocation)
{
    DS_OUT dout;
    
    dout.worldPos = lerp(
		lerp(quad[0].worldPos, quad[1].worldPos, location.x),
		lerp(quad[2].worldPos, quad[3].worldPos, location.x),
		location.y);

    dout.uv = lerp(
		lerp(quad[0].uv, quad[1].uv, location.x),
		lerp(quad[2].uv, quad[3].uv, location.x),
		location.y);

    dout.normal = lerp(
		lerp(quad[0].normal, quad[1].normal, location.x),
		lerp(quad[2].normal, quad[3].normal, location.x),
		location.y);
    
    dout = WaveGeneration(dout);

    dout.clipPos = mul(dout.worldPos, VPMatrix);

    return dout;
}

float4 PS_Main(DS_OUT input) : SV_Target
{
 

    
    float3 lightColor = ComputeLightColor(input.worldPos.xyz, input.normal);
    
    return float4(lightColor, 1.0f) * sea_color;
  
}
