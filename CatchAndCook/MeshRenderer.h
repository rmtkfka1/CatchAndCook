#pragma once
#include "Component.h"
#include "RendererBase.h"

class Material;
class Mesh;
class Shader;

class MeshRenderer : public Component, public RendererBase
{
public:
	~MeshRenderer() override;

	//Component
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;

	//RendererBase
	void Rendering(Material* material, Mesh* mesh) override;
	void DebugRendering() override;

	//MeshRenderer
	void AddMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	std::vector<std::shared_ptr<Material>>& GetMaterials();
	void SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void SetDebugShader(shared_ptr<Shader>& shader) { _normalDebugShader = shader; }

	void SetInstance(InstanceBufferContainer* _instance);
	bool HasInstance() const
	{
		return _hasInstance;
	};

private:
	std::vector<std::shared_ptr<Mesh>> _mesh;
	std::vector<std::shared_ptr<Material>> _uniqueMaterials;
	std::vector<std::shared_ptr<Material>> _sharedMaterials;
	
	shared_ptr<Shader> _normalDebugShader;

	InstanceBufferContainer* _instanceBuffer;
	bool _hasInstance = false;
};

