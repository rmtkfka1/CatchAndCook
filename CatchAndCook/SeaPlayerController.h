﻿#pragma once
#include "Component.h"
class Camera;
class Terrain;
class AnimationListComponent;
class SkinnedHierarchy;
class CableComponent;

enum class SeaPlayerState
{
	Idle,
	Aiming,
	Attack,
	Skill,
	PushBack,
	Die,
	Hit,
};


class SeaPlayerController :public Component
{

public:
	SeaPlayerController();
	virtual ~SeaPlayerController();
	virtual void Init();
	virtual void Start();
	virtual void Update();
	void UpdatePlayerAndCamera(float dt, Quaternion& playerRotation, Quaternion& cameraRotation);
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
	shared_ptr<SkinnedHierarchy> _skined;
	shared_ptr<CableComponent> _weapons;

	std::unordered_map<string, std::shared_ptr<Animation>> _animations;
private:
	weak_ptr<GameObject> _other;
	SeaPlayerState _state = SeaPlayerState::Idle;
	vec3 _velocity = vec3::Zero;

	float _cameraPitchOffset = 2.212f;
	float _cameraForwardOffset = 0.541f;
	float _cameraHeightOffset = 4.130f;
	float  _yaw = 0;
	float _pitch = 0;
	float _roll = 0;

	const float _moveForce = 300.0f;
	const float _maxSpeed = 900.0f;
	const float _resistance = 2.5f;
	const float _playerRadius = 10.0f;

private:


};

