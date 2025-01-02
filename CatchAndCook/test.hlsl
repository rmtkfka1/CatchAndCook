
cbuffer test : register(b1)
{
    float3 testing;

}

struct VS_IN
{
    float3 pos : POSITION;
    //float2 uv : TEXCOORD;
    //float3 normal : NORMAL;
};

struct VS_OUT
{
    float4 pos : SV_Position;

};


VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;

    output.pos = float4(input.pos, 1.0f);
    output.pos.x += testing.x;
    
    return output;
}

float4 PS_Main(VS_OUT input) : SV_Target
{

    return float4(1, 1, 0, 0);
}