#pragma once
#include "Component.h"
#include "SkinnedHierarchy.h"

class SkinnedMeshRenderer : public Component, public  RendererBase
{
public:

	~SkinnedMeshRenderer() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

	void SetModel(const std::shared_ptr<Model>& model);
	void Rendering(Material* material, Mesh* mesh) override;
	void DebugRendering() override;


	void AddMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	std::vector<std::shared_ptr<Material>>& GetMaterials();
	void SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void SetDebugShader(shared_ptr<Shader>& shader) {
		_normalDebugShader = shader;
	}

	void SetBoneRootName(const std::wstring& name) { _boneName = name; };

private:
	std::shared_ptr<Model> _model;
	std::vector<std::shared_ptr<Mesh>> _mesh;
	std::vector<std::shared_ptr<Material>> _uniqueMaterials;
	std::vector<std::shared_ptr<Material>> _sharedMaterials;

	shared_ptr<Shader> _normalDebugShader;

	std::wstring _boneName;
	std::weak_ptr<SkinnedHierarchy> _hierarchy;

	friend class SkinnedHierarchy;
};

