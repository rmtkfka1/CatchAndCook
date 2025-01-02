
cbuffer test : register(b1)
{
    float3 testing;

}

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;

    //float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;

};


Texture2D g_tex_0 : register(t0);
SamplerState g_sam_0 : register(s0);

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.0f);
    output.pos.x += testing.x;
    output.pos.y += testing.y;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

    return g_tex_0.Sample(g_sam_0, input.uv);

}