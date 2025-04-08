#ifndef _LIGHTING_HLSL_
#define _LIGHTING_HLSL_


#define MAX_LIGHTS 20



struct LightMateiral
{
    float3 ambient;
    float shininess;
    
    float3 diffuse;
    int lightType;
    
    float3 specular;
    float dummy;
};

struct Light
{
    LightMateiral mateiral;
    
    float3 strength;
    float fallOffStart;
    
    float3 direction;
    float fallOffEnd;
    
    float3 position;
    float spotPower;

    float dummy2;
	float spotAngle;
    float innerSpotAngle;
    float intensity;

    int onOff;
    float3 dummy1;
};


cbuffer LightParams : register(b3)
{
    float3 g_eyeWorld;
    int g_lightCount;

    Light g_lights[MAX_LIGHTS];

    int useRim = 1;
    float3 rimColor;

    float rimPower = 23.0f;
    float rimStrength = 500.0f;
    float dummy1 = 0;
    float dummy2 = 0;
};

float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
    // 1
    return clamp((d - falloffStart) / (falloffEnd - falloffStart), 0, 1);
}



float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, LightMateiral mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = max(dot(halfway, normal), 0.0f);

    // ½ºÆåÅ§·¯ ¹Ý»ç (±âº» Èò»ö À¯Áö)
    float3 specular = mat.specular * pow(hdotn, mat.shininess);

    // ºû °­µµ´Â diffuse¿¡¸¸ °öÇÏ°í, specular´Â µû·Î Ãß°¡
    float3 diffuse = mat.diffuse * lightStrength;
    
    return (mat.ambient + diffuse) + specular;
}

float3 Phong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, LightMateiral mat)
{
    float3 reflectVec = reflect(-lightVec, normal);
    float rdotv = max(dot(reflectVec, normalize(toEye)), 0.0f);
    float3 specular = mat.specular * pow(rdotv, mat.shininess);
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
}

float3 ComputeDirectionalLight(Light L, LightMateiral mat,float3 worldPos, float3 normal, float3 toEye)
{
    //float3 lightVec = normalize(L.position - worldPos);
    float3 lightVec = normalize(-L.direction);
    float ndotl = max(dot(normal, lightVec), 0.0f);
    float3 LightStrength = L.strength * L.intensity * ndotl;
    return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
}

float3 ComputePointLight(Light L, LightMateiral mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;

    float d = length(lightVec);

    if (d > L.fallOffEnd)
    {
        return float3(0.0, 0.0, 0.0);
    }
    else
    {
     
        lightVec /= d;
        
        float ndotl = saturate(dot(normal, lightVec));
        
        float3 LightStrength = L.strength * ndotl * sqrt(L.intensity) / d; // * ndotl   *  sqrt(L.intensity / (d * d))
        
        float att = 1 - CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);

        LightStrength *= att;
        
        //return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
        return mat.diffuse * LightStrength;

    }
}

float3 ComputeSpotLight(Light L, LightMateiral mat, float3 pos, float3 normal, float3 toEye)
{
    float3 lightVec = L.position - pos;

   
    float d = length(lightVec);

    if (d > L.fallOffEnd)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        lightVec /= d;
        
        float ndotl = saturate(dot(normal, lightVec));
        
        float3 LightStrength = L.strength * ndotl * sqrt(L.intensity) / d; // * ndotl   *  sqrt(L.intensity / (d * d))
        
        float att = 1 - CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        LightStrength *= att;
        
        //float spotFactor = pow(saturate(dot(-lightVec, L.direction)), L.spotPower);
        //LightStrength *= spotFactor;

        float cosThreshold = cos(L.spotAngle/2);
        float cosInnerThreshold = cos(L.innerSpotAngle/2);
		float spotFactor = saturate((dot(-lightVec, L.direction) - cosThreshold) / (cosInnerThreshold - cosThreshold) );
        LightStrength *= spotFactor;
        
        //return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
        return mat.diffuse * LightStrength;
    }
}

float4 ComputeLightColor(float3 worldPos ,float3 WorldNomral)
{
    float3 lightColor = float3(0, 0, 0);

    float3 toEye = normalize(g_eyeWorld - worldPos.xyz);

    [unroll]
    for (int i = 0; i < g_lightCount; ++i)
    {
        if (g_lights[i].onOff == 1)
        {
	        if (g_lights[i].mateiral.lightType == 0)
	        {
	            lightColor += ComputeDirectionalLight(g_lights[i], g_lights[i].mateiral, worldPos, WorldNomral, toEye);
	        }
	        else if (g_lights[i].mateiral.lightType == 1)
	        {
	            lightColor += ComputePointLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye);
	        }
	        else if (g_lights[i].mateiral.lightType == 2)
	        {
	            lightColor += ComputeSpotLight(g_lights[i], g_lights[i].mateiral, worldPos.xyz, WorldNomral, toEye);
	        }
		}
    }
    
    return float4(lightColor, 1.0f);

}

#endif

