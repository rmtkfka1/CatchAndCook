#pragma once
#include "Component.h"


class CameraComponent;

enum class MoveType
{
	Field, Water
};

struct ControlInfo
{
	float cameraRotationSmooth = 40;
	float cameraMoveSmooth = 60;

	float moveForce = 0.32f; // speed * drug 수치만큼은 나와야 MAX를 찍음.
	float maxSpeed = 8.0f;
	float drag = 0.04f;
};

const ControlInfo fieldInfo = {
	40,
	60,
	7,
	8,
	0.25,
};
const ControlInfo waterInfo = {
	100,
	100,
	0.32,
	6,
	0.04,
};

class PlayerController : public Component
{
public:
	std::weak_ptr<CameraComponent> camera;

	~PlayerController() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;

public:
	void SetMoveType(MoveType moveType);
	void CameraControl();
	void MoveControl();

private: // Camera Control
	Vector3 _targetOffset;
	Vector3 _currentOffset;

	Vector3 _targetEuler;
	Vector3 _currentEuler;

	Vector3 targetLookWorldDirection;
	Quaternion currentLookWorldRotation;

private:
	Vector3 velocity = Vector3::Zero;
	MoveType moveType = MoveType::Field;
	ControlInfo controlInfo = fieldInfo;
};

