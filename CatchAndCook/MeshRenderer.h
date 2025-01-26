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

	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void DebugRendering() override;
	void SetDestroy() override;

	void Rendering(Material* material, Mesh* mesh) override;

	void AddMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);

	void SetDrawNormal(bool draw) { _drawNormal = draw; }

private:
	std::vector<std::shared_ptr<Mesh>> _mesh;
	std::vector<std::shared_ptr<Material>> _uniqueMaterials;
	std::vector<std::shared_ptr<Material>> _sharedMaterials;

	bool _drawNormal = true;
	static shared_ptr<Shader> _normalDebugShader;


};

