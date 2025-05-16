
#include "Global_b0.hlsl"
#include "Camera_b2.hlsl"


cbuffer VolumetricData : register(b5)
{
    float3 fogColor;
    float phaseG;
    
    float3 lightDir;
    float waterHeight;
    
    float absorption;
    int numSlices;
    int numSteps;
    float stepSize;
};


struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VS_OUT VS_Main(uint vid : SV_VertexID)
{
    VS_OUT o;

    float2 verts[4] =
    {
        float2(-1, 1), // top-left
        float2(1, 1), // top-right
        float2(-1, -1), // bottom-left
        float2(1, -1) 
    };

    float2 uvs[4] =
    {
        float2(0, 0),
        float2(1, 0),
        float2(0, 1),
        float2(1, 1)
    };

    o.pos = float4(verts[vid], 0.0f, 1.0f);
    o.uv = uvs[vid];

    return o;
}

float PhaseHG(float cosTheta, float g)
{
    float g2 = g * g;
    return (1.0f - g2) / pow(1.0f + g2 - 2.0f * g * cosTheta, 1.5f) * (1.0 / (4 * 3.14159));
}

float3 ProjToView(float2 uvCoord)
{
    float4 posProj;
    posProj.xy = uvCoord * 2.0f - 1.0f;
    posProj.y *= -1;
    posProj.z = 1.0f;
    posProj.w = 1.0f;
    
    float4 posView = mul(posProj, InvertProjectionMatrix);
    return posView.xyz / posView.w;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    // View 공간 기준의 ray 방향 구하기
    float3 viewRayEnd = ProjToView(input.uv);
    float3 viewDir = normalize(viewRayEnd); // 정규화된 뷰 방향
    float3 origin = float3(0, 0, 0); // ViewSpace에서 카메라 위치

    float3 accum = float3(0, 0, 0); // 누적 색상
    float transmittance = 1.0f; // 남은 투과도

    for (int i = 0; i < numSteps; ++i)
    {
        float t = stepSize * i;
        float3 pos = origin + viewDir * t;

        float depth = pos.z; 
        float density = saturate((waterHeight - depth) / waterHeight);

        float3 lightVS = normalize(mul((float3x3) ViewMatrix, lightDir));
        float3 sampleViewDir = normalize(-pos);
        float phase = PhaseHG(dot(sampleViewDir, lightVS), phaseG);

        float attenuation = exp(-absorption * t);
        float3 fogSample = fogColor ;

        accum += fogSample * transmittance;
        transmittance *= (1.0f - density * 0.04f);

        if (transmittance < 0.01f)
            break;
    }

    return float4(saturate(accum), 1.0f - transmittance);
}