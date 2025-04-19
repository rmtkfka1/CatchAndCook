#pragma once
#include "Component.h"
class Camera;
class Terrain;

enum class SeaPlayerState
{
	Idle,
	Move,
	Attack,
	Skill,
	PushBack,
	Die,
	Hit,
};

class Animator;

class SeaPlayerController :public Component
{

public:
	SeaPlayerController();
	virtual ~SeaPlayerController();
	virtual void Init();
	virtual void Start();
	virtual void Update();
	void UpdatePlayerAndCamera(float dt, Quaternion& rotation);
	void KeyUpdate(vec3& inputDir, Quaternion& rotation, float dt);
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void RenderBegin();
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current);
	virtual void SetDestroy();
	virtual void Destroy();

private:
	Quaternion CalCulateYawPitchRoll();

private:
	void UpdateState(float dt);
	void SetState(SeaPlayerState state);

private:
	void Idle(float dt);
	void Move(float dt);
	void Attack(float dt);



private:
	shared_ptr<Transform> _transform;
	shared_ptr<Camera> _camera;
	shared_ptr<Collider> _collider;
	shared_ptr<Terrain> _terrian;

private:
	weak_ptr<GameObject> _other;
	SeaPlayerState _state = SeaPlayerState::Idle;
	vec3 _velocity = vec3::Zero;

	float _cameraForwardOffset = 0.2f;
	float _cameraHeightOffset = 1.2f;
	float  _yaw = 0;
	float _pitch = 0;
	float _roll = 0;

	const float _moveForce = 150.0f;
	const float _maxSpeed = 600.0f;
	const float _resistance = 2.5f;
	const float _playerRadius = 2.5f;

private:


};

