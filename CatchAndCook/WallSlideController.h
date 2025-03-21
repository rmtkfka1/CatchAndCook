#pragma once
#include "Component.h"
class WallSlideController :public Component
{


public:
	WallSlideController();
	virtual ~WallSlideController() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;

private:
	bool _onCollision = false;
	shared_ptr<Collider> _collider;
	shared_ptr<Collider> _other;
	

};

