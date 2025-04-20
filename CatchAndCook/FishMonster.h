#pragma once
#include "Component.h"
#include "PathFinder.h"


class AnimationListComponent;

enum class FishMonsterState
{
	Idle,
	Move,
	Attack,
	Die,
	Hit,
};


class FishMonster :public Component
{
public:
	FishMonster();
	virtual ~FishMonster();
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

public:
	void ReadPathFile(const std::wstring& fileName);

private:
	void UpdateState(float dt);
	void SetState(FishMonsterState state);
	void Idle(float dt);
	void Move(float dt);
	void Attack(float dt);
	void Die(float dt);
	void Hit(float dt);

private:
	vector<vec3> _fishPath;
	bool _forward =true;
	int _currentIndex = 0;
	float _moveSpeed = 100.0f;
	float _distanceMoved = 0.0f;
	float _segmentLength = 0.0f;
	wstring _pathName = L"Null";
	Quaternion _firstQuat;

private:
	shared_ptr<AnimationListComponent> _animList;
	FishMonsterState _state = FishMonsterState::Move;
};

