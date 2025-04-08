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
    float gSurfaceEpsilon = 0.05;
    float gOcclusionFadeStart = 0.2;
    float gOcclusionFadeEnd = 0.2;
	if(distZ > gSurfaceEpsilon)
		occlusion = saturate( (gOcclusionFadeEnd - distZ) / (gOcclusionFadeEnd - gOcclusionFadeStart) );
	
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
    float3 posWS = PositionT.Load(int3(texCoord, 0)).xyz;

    float3 normalVS = normalize(mul(normal, (float3x3)ViewMatrix));
    float4 posVS = mul(float4(posWS, 1.0), ViewMatrix);
    float depthVS = NdcDepthToViewDepth(depth);
    posVS = posVS / posVS.w;

    float3 p = (depthVS/posVS.z) * posVS;


    int kernelSize = 16;

    // SSAO용 반구 내 샘플 벡터 (상수 배열)
	static const float3 sampleKernel[16] = {
	    float3(-0.1392, -0.9657, -0.2208),
	    float3(0.7393, 0.5051, -0.4468),
	    float3(0.0957, -0.1436, 0.9849),
	    float3(0.7894, 0.2286, 0.5707),
	    float3(-0.0263, 0.8501, -0.5260),
	    float3(-0.3846, -0.3665, 0.8472),
	    float3(-0.5411, 0.7505, 0.3806),
	    float3(0.1929, -0.6776, -0.7093),
	    float3(0.2148, 0.8233, 0.5251),
	    float3(-0.8032, 0.1084, -0.5855),
	    float3(0.5173, -0.6829, -0.5144),
	    float3(-0.2836, 0.3437, 0.8957),
	    float3(-0.4712, -0.0945, -0.8769),
	    float3(0.9461, 0.1915, -0.2606),
	    float3(-0.6822, 0.6235, -0.3818),
	    float3(0.1376, -0.9849, 0.1054)
	};

    float gOcclusionRadius = 0.25;
    float occlusionSum = 0;
    for(int i = 0; i < kernelSize; ++i)
	{
		// Are offset vectors are fixed and uniformly distributed (so that our offset vectors
		// do not clump in the same direction).  If we reflect them about a random vector
		// then we get a random uniform distribution of offset vectors.
		float3 offset = sampleKernel[i].xyz;
	
		// Flip offset vector if it is behind the plane defined by (p, n).
		float flip = sign( dot(offset, normalVS) );
		
		// Sample a point near p within the occlusion radius.
		float3 q = p + flip * gOcclusionRadius * offset;
		
		// Project q and generate projective tex-coords.  
		float4 projQ = mul(float4(q, 1.0f), ProjectionMatrix);
		projQ /= projQ.w;

		// Find the nearest depth value along the ray from the eye to q (this is not
		// the depth of q, as q is just an arbitrary point near p and might
		// occupy empty space).  To find the nearest depth we look it up in the depthmap.
		projQ.y *= -1;
		projQ.xy = projQ.xy * 0.5 + 0.5;
		if (projQ.x < 0 || projQ.x > 1 || projQ.y < 0 || projQ.y > 1)
            continue;
		float rz = depthT.Load(int3((float2(projQ.xy) * cameraScreenData.xy) - 0.5, 0));
        rz = NdcDepthToViewDepth(rz);

		// Reconstruct full view space position r = (rx,ry,rz).  We know r
		// lies on the ray of q, so there exists a t such that r = t*q.
		// r.z = t*q.z ==> t = r.z / q.z

		float3 r = (rz / q.z) * q;


		
		//
		// Test whether r occludes p.
		//   * The product dot(n, normalize(r - p)) measures how much in front
		//     of the plane(p,n) the occluder point r is.  The more in front it is, the
		//     more occlusion weight we give it.  This also prevents self shadowing where 
		//     a point r on an angled plane (p,n) could give a false occlusion since they
		//     have different depth values with respect to the eye.
		//   * The weight of the occlusion is scaled based on how far the occluder is from
		//     the point we are computing the occlusion of.  If the occluder r is far away
		//     from p, then it does not occlude it.
		// 
		
		float distZ = p.z - r.z;
		float dp = saturate(dot(normalVS, normalize(r - p)));

        float occlusion = dp * OcclusionFunction(distZ);

		occlusionSum += occlusion;
	}

    occlusionSum /= kernelSize;
	
	float access = 1.0f - occlusionSum;

    //int2 sampleCoord = int2(sampleScreenUV * cameraScreenData.xy + 0.5);

    resultTexture[texCoord] = saturate(pow(access, 6.0f));
	//resultTexture[texCoord] = float4(normal, 1);
    //resultTexture[texCoord] = float4(occlusion, occlusion, occlusion, 1.0);
}


