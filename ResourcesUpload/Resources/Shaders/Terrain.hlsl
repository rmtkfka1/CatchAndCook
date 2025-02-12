#include "GLOBAL.hlsl"
#include  "Light.hlsl"

Texture2D heightMap : register(t0);

SamplerState sample_lerp : register(s0);
SamplerState sample_point : register(s1);
SamplerState sample_aniso4 : register(s2);
SamplerState sample_aniso8 : register(s3);
SamplerState sample_aniso16 : register(s4);
SamplerState sample_shadow : register(s5);


#define TessFactor 8
#define PI 3.14159f
#define DIST_MAX 3000.0f
#define DIST_MIN 3.0f


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

cbuffer TerrainDetailsParam : register(b10)
{
	float3 fieldSize;
    float padding0;
	int detailsCount;
    float2 padding2;
	int blendCount;
    float2 padding3;
	float4 tileST[12];
    float4  textureActive[12];
	//Vector4 color;
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
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

struct DS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

struct HS_OUT
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD;
    float2 uvTile : TEXCOORD1;
    float3 normal : NORMAL;
};

Texture2D _detailMap0 :     register(t32);
Texture2D _detailMap1 :     register(t33);
Texture2D _detailMap2 :     register(t34);
Texture2D _detailMap3 :     register(t35);

Texture2D _detailMap4 :     register(t36);
Texture2D _detailMap5 :     register(t37);
Texture2D _detailMap6 :     register(t38);
Texture2D _detailMap7 :     register(t39);

Texture2D _detailMap8 :     register(t40);
Texture2D _detailMap9 :     register(t41);
Texture2D _detailMap10 :    register(t42);
Texture2D _detailMap11 :    register(t43);

Texture2D _normalMap0 :     register(t44);
Texture2D _normalMap1 :     register(t45);
Texture2D _normalMap2 :     register(t46);
Texture2D _normalMap3 :     register(t47);
                                      
Texture2D _normalMap4 :     register(t48);
Texture2D _normalMap5 :     register(t49);
Texture2D _normalMap6 :     register(t50);
Texture2D _normalMap7 :     register(t51);
                                      
Texture2D _normalMap8 :     register(t52);
Texture2D _normalMap9 :     register(t53);
Texture2D _normalMap10 :    register(t53);
Texture2D _normalMap11 :    register(t55);

Texture2D _maskMap0 :     register(t56);
Texture2D _maskMap1 :     register(t57);
Texture2D _maskMap2 :     register(t58);
Texture2D _maskMap3 :     register(t59);
           
Texture2D _maskMap4 :     register(t60);
Texture2D _maskMap5 :     register(t61);
Texture2D _maskMap6 :     register(t62);
Texture2D _maskMap7 :     register(t63);
                        
Texture2D _maskMap8 :     register(t64);
Texture2D _maskMap9 :     register(t65);
Texture2D _maskMap10 :    register(t66);
Texture2D _maskMap11 :    register(t67);


Texture2D _blendMap0 :      register(t68);
Texture2D _blendMap1 :      register(t69);
Texture2D _blendMap2 :      register(t70);



VS_OUT VS_Main(VS_IN input)
{
    VS_OUT output = (VS_OUT) 0;
    
    output.pos = mul(float4(input.pos, 1.0f), WorldMat);
    output.normal = mul(float4(input.normal, 0.0f), WorldMat).xyz;
    output.uv = input.uv;
    output.uvTile = input.uv * fieldSize.xz;
    
    return output;
}

struct PatchConstOutput
{
    float edges[3] : SV_TessFactor;
    float inside[1] : SV_InsideTessFactor;
};



float CalculateTessLevel(float3 cameraWorldPos, float3 patchPos, float min, float max, float maxLv)
{
    float distance = length(patchPos - cameraWorldPos);
    float ratio = smoothstep(min, max, distance);
    
    float level = lerp(maxLv, 1.0f, ratio);

    return clamp(level, 1.0f, maxLv);
}


//패치단위로 호출
PatchConstOutput ConstantHS(InputPatch<VS_OUT, 3> patch, uint patchID : SV_PrimitiveID)
{

    PatchConstOutput pt;
    
    float3 edge0Pos = (patch[1].pos + patch[2].pos) / 2.f;
    float3 edge1Pos = (patch[2].pos + patch[0].pos) / 2.f;
    float3 edge2Pos = (patch[0].pos + patch[1].pos) / 2.f;

    float edge0TessLevel = CalculateTessLevel(cameraPos.xyz, edge0Pos, DIST_MIN, DIST_MAX, TessFactor);
    float edge1TessLevel = CalculateTessLevel(cameraPos.xyz, edge1Pos, DIST_MIN, DIST_MAX, TessFactor);
    float edge2TessLevel = CalculateTessLevel(cameraPos.xyz, edge2Pos, DIST_MIN, DIST_MAX, TessFactor);

    pt.edges[0] = edge0TessLevel;
    pt.edges[1] = edge1TessLevel;
    pt.edges[2] = edge2TessLevel;
    pt.inside[0] = edge2TessLevel;
    return pt;
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64)]
HS_OUT HS_Main(InputPatch<VS_OUT, 3> patch, uint vertexID : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    //4번호출됨.
    HS_OUT hout;
    hout.pos = patch[vertexID].pos;
    hout.uv = patch[vertexID].uv;
    hout.normal = patch[vertexID].normal;
    hout.uvTile = patch[vertexID].uvTile;
    
    return hout;
}



[domain("tri")]
DS_OUT DS_Main(OutputPatch<HS_OUT, 3> quad, PatchConstOutput patchConst, float3 location : SV_DomainLocation)
{
    DS_OUT dout;
    
    float3 pos = quad[0].pos * location.x + quad[1].pos * location.y + quad[2].pos * location.z;
    float2 uv = quad[0].uv * location.x + quad[1].uv * location.y + quad[2].uv * location.z;
    float2 uvTile = quad[0].uvTile * location.x + quad[1].uvTile * location.y + quad[2].uvTile * location.z;
    float3 normal = quad[0].normal * location.x + quad[1].normal * location.y + quad[2].normal * location.z;
    
    pos.y += heightMap.SampleLevel(sample_point, uv, 0).r * fieldSize.y;
    
    dout.uv = uv;
    dout.pos = mul(float4(pos, 1.0f), VPMatrix);
    dout.normal = normal;
    dout.uvTile = uvTile;
    
    return dout;
}


float4 PS_Main(DS_OUT input) : SV_Target
{
    float4 finalColor = float4(0,0,0,0);
	float4 blend;

    if (blendCount <= 1)
    {
		blend = _blendMap0.Sample(sample_lerp, input.uv);
        float2 tileUV0 = input.uvTile / tileST[0].xy + tileST[0].zw;
        float2 tileUV1 = input.uvTile / tileST[1].xy + tileST[1].zw;
    	float2 tileUV2 = input.uvTile / tileST[2].xy + tileST[2].zw;
    	float2 tileUV3 = input.uvTile / tileST[3].xy + tileST[3].zw;
        float4 mask0 = (textureActive[0].g == 0) ? 1 : (_maskMap0.Sample(sample_lerp, tileUV0));
    	float4 mask1 = (textureActive[1].g == 0) ? 1 : (_maskMap1.Sample(sample_lerp, tileUV1));
    	float4 mask2 = (textureActive[2].g == 0) ? 1 : (_maskMap2.Sample(sample_lerp, tileUV2));
    	float4 mask3 = (textureActive[3].g == 0) ? 1 : (_maskMap3.Sample(sample_lerp, tileUV3));

    	finalColor += _detailMap0.Sample(sample_lerp, tileUV0) * mask0.g * blend.x;
        finalColor += _detailMap1.Sample(sample_lerp, tileUV1) * mask1.g * blend.y;
        finalColor += _detailMap2.Sample(sample_lerp, tileUV2) * mask2.g * blend.z;
        finalColor += _detailMap3.Sample(sample_lerp, tileUV3) * mask3.g * blend.w;
    }
    if (blendCount <= 2)
    {
        blend = _blendMap1.Sample(sample_lerp, input.uv);
        float2 tileUV0 = input.uvTile / tileST[4].xy + tileST[4].zw;
        float2 tileUV1 = input.uvTile / tileST[5].xy + tileST[5].zw;
    	float2 tileUV2 = input.uvTile / tileST[6].xy + tileST[6].zw;
    	float2 tileUV3 = input.uvTile / tileST[7].xy + tileST[7].zw;
        float4 mask0 =(textureActive[4].g == 0) ? 1 :  (_maskMap4.Sample(sample_lerp, tileUV0));
    	float4 mask1 =(textureActive[5].g == 0) ? 1 :  (_maskMap5.Sample(sample_lerp, tileUV1));
    	float4 mask2 =(textureActive[6].g == 0) ? 1 : (_maskMap6.Sample(sample_lerp, tileUV2) );
    	float4 mask3 =(textureActive[7].g == 0) ? 1 : (_maskMap7.Sample(sample_lerp, tileUV3) );

    	finalColor += _detailMap4.Sample(sample_lerp, tileUV0) * mask0.g * blend.x;
        finalColor += _detailMap5.Sample(sample_lerp, tileUV1) * mask1.g * blend.y;
        finalColor += _detailMap6.Sample(sample_lerp, tileUV2) * mask2.g * blend.z;
        finalColor += _detailMap7.Sample(sample_lerp, tileUV3) * mask3.g * blend.w;
    }
    if (blendCount <= 3)
    {
        blend = _blendMap2.Sample(sample_lerp, input.uv);
        float2 tileUV0 = input.uvTile / tileST[8].xy + tileST[8].zw;
        float2 tileUV1 = input.uvTile / tileST[9].xy + tileST[9].zw;
    	float2 tileUV2 = input.uvTile / tileST[10].xy + tileST[10].zw;
    	float2 tileUV3 = input.uvTile / tileST[11].xy + tileST[11].zw;
        float4 mask0 = (textureActive[8].g == 0) ? 1 : _maskMap8.Sample(sample_lerp, tileUV0);
    	float4 mask1 = (textureActive[9].g == 0) ? 1 : _maskMap9.Sample(sample_lerp, tileUV1);
    	float4 mask2 = (textureActive[10].g == 0) ? 1 :_maskMap10.Sample(sample_lerp, tileUV2);
    	float4 mask3 = (textureActive[11].g == 0) ? 1 :_maskMap11.Sample(sample_lerp, tileUV3);

    	finalColor += _detailMap8.Sample(sample_lerp, tileUV0) * mask0.g * blend.x;
        finalColor += _detailMap9.Sample(sample_lerp, tileUV1) * mask1.g * blend.y;
        finalColor += _detailMap10.Sample(sample_lerp, tileUV2) * mask2.g * blend.z;
        finalColor += _detailMap11.Sample(sample_lerp, tileUV3) * mask3.g * blend.w;
    }
    //blend = _blendMap0.Sample(sample_lerp, input.uv);
    //finalColor = _detailMap0.Sample(sample_lerp, input.uvTile / tileST[0].xy + tileST[0].zw) * blend.x;//  / tileST[0].xy + tileST[0].zw
    return finalColor;
}
