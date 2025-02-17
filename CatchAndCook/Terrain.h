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


	void SetMaterial(const shared_ptr<Material>& material){
		_material = material;
	}
	void SetInstances(const vector<shared_ptr<GameObject>>& instances){
		for(auto& instance : instances)
			_instances.push_back(instance);
	}
	void SetInstanceDatas(const vector<vector<Instance_Transform>>& instancesDatas){
		_instanceDatas = instancesDatas;
	}

public:
	void SetHeightMap(const std::wstring &rawPath,const std::wstring &pngPath,const vec2& rawSize,const vec3& fieldSize);
	float GetHeight(const Vector2& heightMapPosition) const;
	float GetLocalHeight(const Vector3& localPosition);
	float GetWorldHeight(const Vector3& worldPosition);

private:
	void LoadTerrain(const std::wstring& rawData);
	void Smooth();
	bool InBounds(int32 z, int32 x) const;
	float Average(int32 i,int32 j);
private:
	static const int CellsPerPatch = 32;

	shared_ptr<Mesh> _gridMesh{};
	shared_ptr<Texture> _heightTexture{};
	Vector3 _fieldSize;
	Vector2 _heightRawSize;
	Vector2 _heightTextureSize;

	shared_ptr<Material> _material;
	vector<weak_ptr<GameObject>> _instances;
	vector<vector<Instance_Transform>> _instanceDatas;
	vector<InstanceBufferContainer*> _instanceBuffers;

	vector<vector<float>> _heightRawMapData;
};

