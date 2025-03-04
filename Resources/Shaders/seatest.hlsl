#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"

#include  "Light_b3.hlsl"

Texture2D _baseMap : register(t0);
Texture2D _bumpMap : register(t1);

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
    const int waveCount = 32; // 파도 개수 증가
    float baseWavelength = 150.0f; // 초기 파장 증가 (더 큰 파도)
    float maxAmplitude = 2.5f; // 전체적으로 낮은 진폭
    float steepnessFactor = 0.2f; // 기본적인 파도의 기울기

    float3 modifiedPos = input.worldPos.xyz;
    float dHdX = 0.0f;
    float dHdZ = 0.0f;

    for (int i = 0; i < waveCount; i++)
    {
        // 랜덤한 방향 생성 (이전보다 더 무작위하게)
        float randomAngle = frac(sin(i * 837.15f) * 43758.5453f) * 2.0f * PI;
        float2 direction = normalize(float2(cos(randomAngle), sin(randomAngle)));

        // 파장과 주파수 조절 (멀리 갈수록 파장이 길어지도록)
        float wavelength = baseWavelength * pow(1.25, i / 8.0f); // 8개 단위로 천천히 증가
        float frequency = 2 * PI / wavelength;

        // 파도의 크기 조절 (낮은 주파수의 파도가 강하게)
        float amplitude = maxAmplitude * pow(0.8f, i / 5.0f); // 큰 파도는 오래 유지됨
        float speed = 0.2f + 0.015f * i; // 작은 파도가 더 빠르게 이동

        // Steepness 조절 (큰 파도는 완만하게, 작은 파도는 날카롭게)
        float steepness = steepnessFactor * (1.0f - pow(0.9f, i));

        // 파동 계산
        float dotProduct = dot(direction, input.worldPos.xz);
        float wave = sin(dotProduct * frequency + speed * g_Time);
        float waveDerivative = cos(dotProduct * frequency + speed * g_Time);

        // Gerstner Waves 적용 (수면의 움직임)
        modifiedPos.x += steepness * amplitude * direction.x * waveDerivative;
        modifiedPos.z += steepness * amplitude * direction.y * waveDerivative;
        modifiedPos.y += amplitude * wave;

        // 편미분 계산 (법선 벡터 업데이트)
        float dWavedX = frequency * waveDerivative * direction.x;
        float dWavedZ = frequency * waveDerivative * direction.y;

        dHdX += amplitude * dWavedX;
        dHdZ += amplitude * dWavedZ;
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
    
    //dout = WaveGeneration(dout);
    
    float3 tagent = float3(1, 0, 0);
    
    float4 map = _bumpMap.SampleLevel(sampler_lerp, dout.uv,0);
    
    float3 N = normalize(dout.normal); 
    float3 T = normalize(tagent); 
    float3 B = normalize(cross(N, T)); 
    float3x3 TBN = float3x3(T, B, N); 

    float3 tangentSpaceNormal = (map.rgb * 2.0f - 1.0f);
    float3 worldNormal = mul(tangentSpaceNormal, TBN);

    dout.worldPos.y += worldNormal;
    dout.clipPos = mul(dout.worldPos, VPMatrix);

    return dout;
}

float4 PS_Main(DS_OUT input) : SV_Target
{
    
    ComputeNormalMapping(input.normal, float3(1, 0, 0), input.uv, _bumpMap);
 
    float3 color= ComputeLightColor(input.worldPos.xyz, input.normal);
    
    return float4(color, 1.0f) *sea_color;
  
}
