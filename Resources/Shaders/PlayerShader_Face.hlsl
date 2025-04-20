
#include "Global_b0.hlsl"
#include "Transform_b1.hlsl"
#include "Camera_b2.hlsl"
#include "Light_b3.hlsl"
#include "Skinned_b5.hlsl"


cbuffer DefaultMaterialParam : register(b7)
{
    float4 color = float4(1, 1, 1, 1);
    float4 _baseMapST = float4(1, 1, 1, 1);
};

cbuffer PlayerMaterialParam : register(b8)
{
    float4 temp = float4(1, 1, 1, 1);
};

struct VS_IN
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD0;
    
    #ifdef SKINNED
    float2 uv1 : TEXCOORD1;
    float2 uv2 : TEXCOORD2;
    float4 color : COLOR;
    float4 boneIds : BONEIDs;
    float4 boneWs : BONEWs;
	#endif

};

struct VS_OUT
{
    float4 position : SV_Position;
    float4 positionCS : PositionCS;
    float4 positionWS : PositionWS;
    float3 normalOS : NormalOS;
    float3 normalWS : NormalWS;
    float3 tangentOS : TangentOS;
    float3 tangentWS : TangentWS;
    float2 uv : TEXCOORD0;

    float3 rightWS : NORMAL2;
    float3 forwardWS : NORMAL3;
};

Texture2D _BaseMap : register(t0);
Texture2D _BumpMap : register(t1);

Texture2D _Thinkness : register(t2);
Texture2D _SDFMap : register(t3);
Texture2D _RampSkinMap : register(t4);


Texture2D _BakedGIMap : register(t8);

VS_OUT VS_Main(VS_IN input, uint id : SV_InstanceID)
{
    VS_OUT output = (VS_OUT) 0;

    Instance_Transform transformData = TransformDatas[offset[STRUCTURED_OFFSET(30)].r + id];
    //LightForwardParams lightingData = ForwardLightDatas[offset[STRUCTURED_OFFSET(31)].r + id];

    row_major float4x4 l2wMatrix = transformData.localToWorld;
    row_major float4x4 w2lMatrix = transformData.worldToLocal;
    float4 boneIds = 0;
    float4 boneWs = 0;
    
    
#ifdef SKINNED
	    boneIds = input.boneIds;
	    boneWs = input.boneWs;
#endif
    
    output.normalOS = input.normal;
    output.tangentOS = input.tangent;


    output.positionWS = TransformLocalToWorld(float4(input.pos, 1.0f), boneIds, boneWs, l2wMatrix);
    output.positionCS = TransformWorldToClip(output.positionWS);
    output.position = output.positionCS;

    output.normalWS = TransformNormalLocalToWorld(input.normal, boneIds, boneWs, w2lMatrix);
    output.tangentWS = TransformNormalLocalToWorld(input.tangent, boneIds, boneWs, w2lMatrix);

    output.uv = input.uv;

    output.rightWS = mul(float3(1,0,0), (float3x3)(l2wMatrix));
    output.forwardWS = mul(float3(0,0,1), (float3x3)(l2wMatrix));

    return output;
}



LightingResult ComputeLightColorForward(float3 worldPos ,float3 WorldNomral)
{
    float3 lightColor = float3(0, 0, 0);

    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    LightingResult result = (LightingResult)0;

    //[unroll]
    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].onOff == 1)
        {
	        if (g_lights[i].mateiral.lightType == 0)
	        {
	            ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, worldPos, WorldNomral, toEye, result);
	        }
	        else if (g_lights[i].mateiral.lightType == 1)
	        {
	            ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
	        }
	        else if (g_lights[i].mateiral.lightType == 2)
	        {
	            ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye, result);
	        }
		}
    }
    
    return result;
}

float Sobel(float4 positionCS, float scale)
{
    float2 uv = positionCS.xy / positionCS.w;
    uv.y *= -1;
    uv = uv * 0.5 + 0.5;

	float2 texelSize = scale / g_window_size;

	// 주변 9개 픽셀의 깊이를 샘플링
	float d0 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2(-texelSize.x, -texelSize.y)).r);
	float d1 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2( 0.0f,       -texelSize.y)).r);
	float d2 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2( texelSize.x, -texelSize.y)).r);
	float d3 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2(-texelSize.x,  0.0f       )).r);
	//float d4 = DepthTexture.Sample(sampler_lerp, uv).r;  // 센터 픽셀 (필요시 사용)
	float d5 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2( texelSize.x,  0.0f       )).r);
	float d6 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2(-texelSize.x,  texelSize.y )).r);
	float d7 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2( 0.0f,        texelSize.y )).r);
	float d8 = NdcDepthToViewDepth(DepthTexture.Sample(sampler_lerp, uv + float2( texelSize.x,  texelSize.y )).r);

    float gx = (d2 + 2.0f * d5 + d8) - (d0 + 2.0f * d3 + d6);
    float gy = (d0 + 2.0f * d1 + d2) - (d6 + 2.0f * d7 + d8);
    
    // 기울기의 크기를 계산
    float edgeStrength = length(float2(gx, gy));
    
    // 임계값을 설정하여 엣지 여부 판정 (필요에 따라 조정)
    float threshold = 0.05f; // 이 값을 조정하여 민감도를 변경할 수 있습니다.
    return edgeStrength; //  > threshold ? 1.0f : 0.0f
}

float4 PS_Main(VS_OUT input) : SV_Target
{
    float3 N = ComputeNormalMapping(input.normalWS, input.tangentWS, _BumpMap.Sample(sampler_lerp, input.uv));
    
    LightingResult lightColor = ComputeLightColorForward(input.positionWS.xyz, N);
    
    float4 BaseColor = _BaseMap.Sample(sampler_lerp, input.uv * _baseMapST.xy + _baseMapST.zw) * color;
    float4 ShadowColor = _BakedGIMap.Sample(sampler_lerp_clamp, saturate(dot(float3(0, 1, 0), N) * 0.5 + 0.5));




    //SDF
    float2 l = normalize(lightColor.direction.xz);
    float2 f = normalize(input.forwardWS.xz);
    float2 r = normalize(input.rightWS.xz);

    float angle  = clamp(0, 1, dot(l, f) * 0.5 + 0.5);
    float sdf = dot(l, r) <= 0 ? _SDFMap.Sample(sampler_lerp_clamp, input.uv) : _SDFMap.Sample(sampler_lerp_clamp, float2(1 - input.uv.x, input.uv.y));

    float atten2 = clamp(-1, 1, (angle - sdf) * 2) * lightColor.intensity;
    atten2 = atten2 * 0.5 + 0.5;
    //float3 finalColor = (lerp(ShadowColor * BaseColor, BaseColor, atten2) + float4(lightColor.subColor, 0)).xyz;
    float3 finalColor = (BaseColor * _RampSkinMap.Sample(sampler_lerp_clamp, float2(atten2, 0)) + float4(lightColor.subColor, 0)).xyz;
    finalColor = lerp(ShadowColor * finalColor, finalColor, clamp(0, 1, lightColor.intensity));


    float SSSDistorion = 0.3f;
	float SSSScale = 0.3f;
    float SSSPow = 2.5f;

    // SSS 파트.
    float3 viewDir = normalize(cameraPos - input.positionWS.xyz);
    float3 H = normalize(lightColor.direction + N * SSSDistorion);
    float SSS = pow(saturate(dot(viewDir, -H)), SSSPow) * _Thinkness.Sample(sampler_lerp, input.uv).r;
    //float3 SSSFinal = lerp(float3(1,1,1), float3(1, 0, 0) * SSSScale, SSS);
    float3 SSSFinal =  float3(1, 0.2, 0.1) * SSSScale * SSS;



    // + Sobel(input.positionCS, 3 / input.positionCS.w)

    if (BaseColor.a <= 0.1f)
		discard;
    
    return float4(finalColor, 1) + float4(SSSFinal, 0);
}
