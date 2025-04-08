SamplerState sampler_lerp : register(s0);
SamplerState sampler_point : register(s1);
SamplerState sampler_aniso4 : register(s2);
SamplerState sampler_aniso8 : register(s3);
SamplerState sampler_aniso16 : register(s4);
SamplerState sampler_shadow : register(s5);
SamplerState sampler_lerp_clamp : register(s6);


cbuffer GLOBAL_DATA : register(b0)
{
    float2 g_window_size;
    float g_Time;
    float g_padding;
};

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



float NdcDepthToViewDepth(float z_ndc)
{
    float viewZ = ProjectionMatrix[3][2] / (z_ndc - ProjectionMatrix[2][2]);
    return viewZ;
}
float OcclusionFunction(float distZ)
{
	float occlusion = 0.0f;
    float gSurfaceEpsilon = 0.025;
    float gOcclusionFadeStart = 0;
    float gOcclusionFadeEnd = 0.5;
	if(distZ > gSurfaceEpsilon)
	{
		occlusion = saturate( (gOcclusionFadeEnd - distZ) / gOcclusionFadeEnd - gOcclusionFadeStart );
	}
	
	return occlusion;	
}

RWTexture2D<float4> resultTexture : register(u0);
Texture2D depthT : register(t0); 
Texture2D<float4> RenderT : register(t1);
Texture2D<float4> PositionT : register(t2);
Texture2D<float4> NormalT : register(t3);

[numthreads(16, 16, 1)]
void CS_Main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    // 현재 픽셀의 화면 좌표와 UV 계산
    int2 texCoord = dispatchThreadID.xy;
    float2 uv = (float2(texCoord) + 0.5) / cameraScreenData.xy;

    // depth texture에서 픽셀의 깊이값(0~1 범위)을 읽어옴
    float depth = depthT.Load(int3(texCoord, 0));
    float3 normal = normalize(NormalT.Load(int3(texCoord, 0)).xyz * 2.0 - 1.0);
    float3 worldPos = PositionT.Load(int3(texCoord, 0)).xyz;
    //float3 normal

        // 재구성을 위해 클립 공간 좌표로 변환 ([0,1] → [-1,1])
    float4 clipPos;
    clipPos.xy = uv * 2.0 - 1.0;
	clipPos.y *= -1;
    clipPos.z = depth;
    clipPos.w = 1.0;

   // InverseProjectionMatrix를 사용하여 뷰 공간 좌표로 변환
    float4 viewPos4 = mul(clipPos, InvertProjectionMatrix);
    viewPos4 /= viewPos4.w;
    float3 viewPos = viewPos4.xyz;


    //float4 viewPos4 = mul(float4(worldPos, 1.0), ViewMatrix);
    //viewPos4 /= viewPos4.w;
    //float3 viewPos = viewPos4.xyz;


    // 뷰 공간 좌표가 유효하지 않으면(예: 배경) 처리
    if(length(viewPos) == 0)
    {
        resultTexture[texCoord] = float4(1, 1, 1, 1);
        return;
    }
    
    float occlusion = 0.0;
    float radius = 0.5;
    float bias = 0.025;
    int kernelSize = 16;

    // SSAO용 반구 내 샘플 벡터 (상수 배열)
    static const float3 sampleKernel[16] = {
         float3(0.5381, 0.1856, 0.4319),
         float3(0.1379, 0.2486, 0.4430),
         float3(0.3371, 0.5679, 0.0057),
         float3(0.7152, 0.3456, 0.2937),
         float3(0.2462, 0.3225, 0.5749),
         float3(0.1343, 0.3111, 0.4546),
         float3(0.7215, 0.4532, 0.2364),
         float3(0.3154, 0.6213, 0.1468),
         float3(0.2346, 0.7452, 0.2983),
         float3(0.5621, 0.2314, 0.1423),
         float3(0.4235, 0.3127, 0.1876),
         float3(0.1876, 0.4123, 0.3127),
         float3(0.3214, 0.1247, 0.4215),
         float3(0.5123, 0.3412, 0.1345),
         float3(0.3127, 0.6543, 0.1234),
         float3(0.4215, 0.2314, 0.3127)
    };

     float3 tangent;
    //if (abs(normal.y) < 0.999)
        //tangent = normalize(cross(normal, float3(0, 1, 0)));
    //else
    tangent = normalize(cross(normal, float3(0, 0, 1)));
    float3 bitangent = cross(tangent, normal);

    // 각 커널 샘플에 대해 오클루전 기여 산출
    for (int i = 0; i < kernelSize; i++)
    {
        // 샘플 분포 스케일 조절 (더 가까운 샘플은 낮은 scale)
        float scale = lerp(0.1, 1.0, (float(i) / float(kernelSize)) * (float(i) / float(kernelSize)));
        float3 sampleDir = normalize(tangent * sampleKernel[i].x +
                                     bitangent * sampleKernel[i].y +
                                     normal    * sampleKernel[i].z);
        // 뷰 공간에서 샘플 위치 계산
        float3 sampleWorldPos = worldPos + sampleDir * radius * scale;

        // 샘플 위치를 VPMatrix를 사용해 클립 공간으로 변환하고, 이후 화면 UV 계산
        float4 sampleViewPos = mul(float4(sampleWorldPos, 1.0), ViewMatrix);
        float4 sampleClipPos = mul(sampleViewPos, ProjectionMatrix);
        sampleClipPos /= sampleClipPos.w;
        sampleClipPos.y *= -1;
        float2 sampleScreenUV = sampleClipPos.xy * 0.5 + 0.5;

        // 화면 범위를 벗어나는 샘플은 무시
        if(sampleScreenUV.x < 0.0 || sampleScreenUV.x > 1.0 ||
           sampleScreenUV.y < 0.0 || sampleScreenUV.y > 1.0)
            continue;

        // sampleUV를 정수 픽셀 좌표로 변환하여 depth texture에서 실제 깊이 읽기
        int2 sampleCoord = int2(sampleScreenUV * cameraScreenData.xy + 0.5);


	    float depth2 = depthT.Load(int3(sampleCoord, 0));
	    float4 clipPos;
	    clipPos.xy = sampleScreenUV * 2.0 - 1.0;
		clipPos.y *= -1;
	    clipPos.z = depth2;
	    clipPos.w = 1.0;

	   // InverseProjectionMatrix를 사용하여 뷰 공간 좌표로 변환
	    float4 viewPos4 = mul(clipPos, InvertProjectionMatrix);
	    viewPos4 /= viewPos4.w;
	    float sampleRealDepth = viewPos4.z;
        

     //   float3 sampleRealWorldPos = PositionT.Load(int3(sampleCoord, 0));

	    //float4 sampleRealViewPos = mul(float4(sampleRealWorldPos, 1), ViewMatrix);
	    //sampleRealViewPos /= sampleRealViewPos.w;
	    //float sampleRealDepth = sampleRealViewPos.z;

        // 샘플 위치의 예상 깊이 (클립 z값을 [0,1]로 재매핑)
        float sampleExpectedDepth = sampleViewPos.z;

        // 뷰 공간 z값의 차이에 따른 가중치 (거리 기반 조절)
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(viewPos.z - sampleViewPos.z));

        // 깊이 비교: depth texture의 값이 예상 깊이보다 크면(즉, 더 멀다면) 오클루전 기여
        occlusion += (sampleRealDepth >= sampleExpectedDepth + bias ? 1.0 : 0.0) * rangeCheck; //  * rangeCheck
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    //occlusion = (occlusion / kernelSize);
    resultTexture[texCoord] = float4(occlusion, occlusion, occlusion, 1.0);
}


