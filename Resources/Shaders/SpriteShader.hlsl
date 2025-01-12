
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
    float2 TexCoord : TEXCOORD;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 TexCoord : TEXCOORD;
};

VS_OUT VS_Main(VS_IN input)
{
    VS_OUT result = (VS_OUT) 0;
    
    float2 scale = g_Scale;
    float2 offset = (g_Pos / g_window_size);
    float2 pos = input.pos.xy * g_Scale + offset;
    result.position = float4(pos.xy * float2(2, -2) + float2(-1, 1), g_depth, 1);
 
    float2 tex_scale = (g_texSampleSize/ g_origintexSize);
    float2 tex_offset = (g_texSamplePos / g_origintexSize);
    
    result.TexCoord = input.TexCoord * tex_scale + tex_offset;
    
    result.color = input.color;
    
    return result;
}


float4 PS_Main(VS_OUT input) : SV_TARGET
{
    float4 texColor = texDiffuse.Sample(samplerDiffuse, input.TexCoord);
    return texColor/* * input.color*/;
}
