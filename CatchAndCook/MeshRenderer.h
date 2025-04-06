﻿#pragma once
#include "Component.h"
#include "RendererBase.h"


class ForwardLightSetter;
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
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;

	//RendererBase
	void Rendering(Material* material, Mesh* mesh, int instanceCount = 1) override;
	void DebugRendering() override;

	//MeshRenderer
	void AddMesh(const std::shared_ptr<Mesh>& _mesh);
	void SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	std::vector<std::shared_ptr<Material>>& GetMaterials();
	void SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials);
	void SetDebugShader(shared_ptr<Shader>& shader) { _normalDebugShader = shader; }

private:
	std::vector<std::shared_ptr<Mesh>> _mesh;
	std::vector<std::shared_ptr<Material>> _uniqueMaterials;
	std::vector<std::shared_ptr<Material>> _sharedMaterials;
	shared_ptr<Shader> _normalDebugShader;


	std::shared_ptr<ForwardLightSetter> _setter_ForwardLight;
};

