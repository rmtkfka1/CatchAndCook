#pragma once
#include "IDelayDestroy.h"
#include "IGuid.h"

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
	void SetOwner(const std::shared_ptr<GameObject>& owner) { this->_owner = owner; };
	std::shared_ptr<GameObject> GetOwner() {return _owner.lock(); };

	bool IsExecuteAble() override;

	virtual void Init(); 
	virtual void Start(); 
	virtual void Update(); 
	virtual void Update2(); 
	virtual void Enable(); 
	virtual void Disable(); 
	virtual void Destroy(); 

	virtual void RenderBegin();

	virtual void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void DebugRendering(); // CBuffer <- ����
	virtual void SetDestroy() override;
	virtual void DestroyComponentOnly();

private:
	int _order = 0;
	std::weak_ptr<GameObject> _owner;
};