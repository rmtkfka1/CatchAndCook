#ifndef _LIGHTING_HLSL_
#define _LIGHTING_HLSL_


#define MAX_LIGHTS 5 



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
    return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, LightMateiral mat)
{
    float3 halfway = normalize(toEye + lightVec);
    float hdotn = dot(halfway, normal);
    float3 specular = mat.specular * pow(max(hdotn, 0.0f), mat.shininess);
    return mat.ambient + (mat.diffuse + specular) * lightStrength;
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
    float3 LightStrength = L.strength * ndotl;
    return Phong(LightStrength, lightVec, normal, toEye, mat);
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
        
        float3 LightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
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
        
        float3 LightStrength = L.strength * ndotl;
        
        float att = CalcAttenuation(d, L.fallOffStart, L.fallOffEnd);
        LightStrength *= att;
        
        float spotFactor = pow(saturate(dot(-lightVec, L.direction)), L.spotPower);
        
        LightStrength *= spotFactor;
        
        //return BlinnPhong(LightStrength, lightVec, normal, toEye, mat);
        return mat.diffuse * LightStrength;
    }
}

#endif