#pragma once
#include "IDelayDestroy.h"
#include "IGuid.h"

class GameObject;
class Collider;

class Component : public IGuid, public IDelayDestroy
{
public:
	Component();
	virtual ~Component() override;
	bool operator<(const Component& other) const;

protected:
	void SetOrder(int order) { _order = order; };
	int GetOrder() const { return _order; };
public:
	bool IsFirst() const { return _first; };
	void FirstOff() { _first = false; };
	// 라이프 사이클에서 돌릴 함수 종류

	void SetOwner(const std::shared_ptr<GameObject>& owner) { this->_owner = owner; };
	std::shared_ptr<GameObject> GetOwner() { assert(!_owner.expired()); return _owner.lock(); };

	virtual void Init(); // 생성될 시 // Create 메모리 잡혔을때.
	virtual void Start(); // 첫 프레임 // 첫 프레임 시.
	virtual void Update(); //
	virtual void Update2(); // 
	virtual void Enable(); // 
	virtual void Disable(); //
	virtual void Destroy(); //

	virtual void RenderBegin();
	virtual void Rendering(); // CBuffer <- 갱신

	// 이벤트 함수
	virtual void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void DebugRendering(); // CBuffer <- 갱신

private:
	bool _first = true;
	int _order = 0;
	std::weak_ptr<GameObject> _owner;
};