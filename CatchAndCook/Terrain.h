#pragma once

#include "Component.h"
#include "RendererBase.h"

class Terrain : public Component,public RenderObjectSetter
{

public:
	Terrain();
	virtual ~Terrain() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

	virtual void PushData() override;
	virtual void SetData(Material* material =nullptr) override;
	bool IsExecuteAble() override;

public:
	void SetHeightMap(const std::wstring& rawData,const std::wstring& pngData, float scale);
	float TerrainGetHeight(float x,float z, float offset);

private:
	void LoadTerrain(const std::wstring& rawData);
	void Smooth();
	bool InBounds(int32 i,int32 j);
	float Average(int32 i,int32 j);
private:
	static const int CellsPerPatch = 32;

	shared_ptr<Mesh> _gridMesh{};
	shared_ptr<Texture> _heightMap{};
	float _gridXsize=0;
	float _gridZsize=0;

	float _heightMapX=0;
	float _heightMapZ=0;


	vector<vector<float>> _heightMapData;
};

