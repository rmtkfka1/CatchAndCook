#pragma once
#include "Component.h"

class Material;
class Mesh;

class MeshRenderer : public Component
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
	void Rendering() override;
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void DebugRendering() override;
	void SetDestroy() override;
	void DestroyComponentOnly() override;

	void SetMesh(const std::shared_ptr<Mesh>& _mesh);
	//기존껄 덮어 쓰기
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	//기존것 유지하면서 추가
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);

private:
	std::shared_ptr<Mesh> _mesh;
	std::vector<std::shared_ptr<Material>> _materials;
};

