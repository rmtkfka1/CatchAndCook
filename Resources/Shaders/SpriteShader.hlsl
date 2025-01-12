
Texture2D texDiffuse : register(t8);
SamplerState samplerDiffuse : register(s0);

cbuffer GLOBAL_DATA : register(b0)
{
    float2  g_window_size;
    float   g_Time;
    float   g_padding;
};

cbuffer CONSTANT_BUFFER_SPRITE : register(b5)
{
 
    float2 g_Pos;
    float2 g_Scale;
    
    float2 g_origintexSize;
    float2 g_texSamplePos;
    
    float2 g_texSampleSize;
    float g_depth;
    float g_alpha;

};

struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    float2 scale = g_Scale;
    float2 offset = (g_Pos / g_window_size);
    float2 pos = input.pos.xy * g_Scale + offset;
    output.pos = float4(pos.xy * float2(2, -2) + float2(-1, 1), g_depth, 1);
 
    float2 tex_scale = (g_texSampleSize/ g_origintexSize);
    float2 tex_offset = (g_texSamplePos / g_origintexSize);
    
    output.uv = input.uv * tex_scale + tex_offset;
    
    output.color = input.color;
    
    return output;
}


float4 PS_Main(VS_OUT input) : SV_TARGET
{
    float4 texColor = texDiffuse.Sample(samplerDiffuse, input.uv);
    return texColor * input.color;
}
