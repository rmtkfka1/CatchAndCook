#pragma once

#include "Component.h"
#include "RendererBase.h"

class Terrain:public Component,public RenderObjectSetter
{

public:
	Terrain(int32 size_x,int32 size_y,int32 division_x,int32 division_y);
	virtual ~Terrain() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void Collision(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

	virtual void PushData() override;
	virtual void SetData(Material* material =nullptr) override;

public:
	void SetHeightMap(const std::wstring& rawData,const std::wstring& pngData);


	void ReadRawData(const std::wstring& path);

private:
	shared_ptr<Mesh> _mesh{};
	shared_ptr<Texture> _heightMap{};

	vec2 _size{};
	WORD** _rawData{};

};

