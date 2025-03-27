#pragma once

#include "Component.h"

class Camera;

class SeaPlayerComponent : public Component
{
public:
	SeaPlayerComponent();
	virtual ~SeaPlayerComponent() override;
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
	float _speed = 10.0f;
private:
	shared_ptr<Camera> _camera;

	vec2 _prevMousePosition;
};

