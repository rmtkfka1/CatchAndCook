#pragma once



struct VolumetricData
{
	vec3 color{};
	float phase =30.0f;

	vec3 lightDir{};
	float waterHeight=2000.0f;

	vec2 padding{};
	float Absorption =5.0f;
	int numSlice=30.0f;

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

