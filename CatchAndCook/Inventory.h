#pragma once

#include "Component.h"
#include "RendererBase.h"

class Sprite;

class Inventory : public Component, public RenderCBufferSetter
{
public:
	Inventory();
	virtual ~Inventory();

private:
	void Init() override;
	void Start() override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;
	void SetData(Material* material = nullptr) override;

public:
	void SetSprite(const std::shared_ptr<Sprite>& sprite) { _InventorySprite = sprite; }
	void SetPos(const vec3& pos);
	void SetSize(const vec2& size);

private:
	shared_ptr<Sprite> _InventorySprite;

};

