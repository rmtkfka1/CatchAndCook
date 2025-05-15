#pragma once



struct VolumetricData
{
	vec3 fogColor; 
	float phaseG; 
	vec3 lightDir;
	float waterHeight;
	float absorption; 
	int numSlices; 
};


class Volumetric
{
public:

	static unique_ptr<Volumetric> main;

	Volumetric();
	~Volumetric();

public:
	void Init();
	void Render();


	shared_ptr<Texture>& GetTexture() { return _texture; }



private:
	shared_ptr<Shader> _shader;
	D3D12_VIEWPORT _viewport;
	D3D12_RECT _scissorRect;
	shared_ptr<Texture> _texture;
	VolumetricData _data;



};

