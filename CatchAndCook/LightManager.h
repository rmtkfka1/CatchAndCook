#pragma once

enum class LIGHT_TYPE
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT,
};

struct LightMaterial
{
	vec3 ambient = vec3(0.0f,0.0f,0.0f);
	float shininess = 32.0f;
	vec3 diffuse = vec3(1.0f,1.0f,1.0f);
	int lightType = 0;
	vec3 specular = vec3(1.0f,1.0f,1.0f);
	float dummy = 0;
};

struct Light
{
	LightMaterial material;
	vec3 strength = vec3(1.0f,1.0f,1.0f);
	float fallOffStart = 0;
	vec3 direction = vec3(0,-1.0f,0);
	float fallOffEnd = 0;
	vec3 position = vec3(0,0,0);
	float spotPower = 64.0f;
};

struct LightParams
{
	vec3 eyeWorldPos{};
	int lightCount = 0;
	Light light[5];

	int useRim = 1;
	vec3 rimColor = vec3(0,1.0f,0);
	float rimPower = 23.0f;
	float rimStrength = 500.0f;
	float dummy1 = 0;
	float dummy2 = 0;
};


class LightManager
{

public:
	static unique_ptr<LightManager> main;

	void PushLight(Light light);
	void SetData();

private:
	void Update();

public:
	LightParams _lightParmas;
};

