#pragma once
#include "RendererBase.h"
#include "Sprite.h"

class Item : public Sprite
{
public:
	Item();
	virtual ~Item();

public:
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


private:

};

