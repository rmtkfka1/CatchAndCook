#pragma once

#include "Component.h"
#include "RendererBase.h"

class Mesh;

struct SpriteStruct
{

};

class SpriteRenderer : public Component, public RendererBase
{

public:
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
	void DestroyComponentOnly() override;

	void Rendering(const std::shared_ptr<Material>& material) override;

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Material> _material;

	RECT _uvMapping;
	vec2 _size;
	vec2 _pos;
};

