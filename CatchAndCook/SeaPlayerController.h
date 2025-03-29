#pragma once
#include "Component.h"
class Camera;

class SeaPlayerController :public Component
{

public:
	SeaPlayerController();
	virtual ~SeaPlayerController();
	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void SetDestroy();
	virtual void Destroy();

	void CalCulateYawPitchRoll();
private:
	shared_ptr<Transform> _transform;
	shared_ptr<Camera> _camera;
	shared_ptr<Collider> _collider;


public:
	vec3 _velocity = vec3::Zero;
	vec2 _centerMousePos;
	float  _yaw = 0;
	float _pitch = 0;
	float _roll = 0;

	const float _moveForce = 30.0f;
	const float _maxSpeed = 30.0f;
	const float _resistance = 2.5f;

};

