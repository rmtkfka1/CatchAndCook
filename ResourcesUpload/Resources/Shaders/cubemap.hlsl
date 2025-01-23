struct VS_IN
{
    float3 localPos : POSITION;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float3 localPos : TEXCOORD;
};

cbuffer Transform : register(b1)
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

    float cameraPos;
    float cameraUp;
    float cameraFrustumData;
    float cameraScreenData;
};

TextureCube g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    // 로컬 좌표를 그대로 전달
    output.localPos = input.localPos;
    // Translation은 하지 않고 Rotation만 적용한다
    float4 clipSpacePos = mul(float4(input.localPos, 0), VPMatrix);
    output.pos = clipSpacePos.xyww;
    

    return output;
}


float4 PS_Main(VS_OUT input) : SV_Target
{
    return g_tex_0.Sample(g_sam_0, input.localPos);  
}