#pragma once
#include "Component.h"

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
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

	void SetModel(const std::shared_ptr<Model>& model);
	void SetMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void Rendering(Material* material, Mesh* mesh) override;
	void DebugRendering() override;

private:
	std::shared_ptr<Model> _model;
	std::shared_ptr<Mesh> _mesh;
	std::vector<std::shared_ptr<Material>> _materials;
};

