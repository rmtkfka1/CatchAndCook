#pragma once

#include "Component.h"
#include "RendererBase.h"

class Sprite;

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
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;

	//RendererBase
	void DebugRendering() override;
	void Rendering(Material* material, Mesh* mesh) override;

public:
	template <typename T, typename = std::enable_if_t<std::is_base_of_v<Sprite, T>>>
	void SetSprite(shared_ptr<T> sprite)
	{
		_sprite = sprite;
	}

private:
	shared_ptr<Sprite> _sprite;

};

