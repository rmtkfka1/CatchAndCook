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
	// ������ ����Ŭ���� ���� �Լ� ����

	void SetOwner(const std::shared_ptr<GameObject>& owner) { this->_owner = owner; };
	std::shared_ptr<GameObject> GetOwner() { assert(!_owner.expired()); return _owner.lock(); };

	virtual void Init(); // ������ �� // Create �޸� ��������.
	virtual void Start(); // ù ������ // ù ������ ��.
	virtual void Update(); //
	virtual void Update2(); // 
	virtual void Enable(); // 
	virtual void Disable(); //
	virtual void Destroy(); //

	virtual void RenderBegin();
	virtual void Rendering(); // CBuffer <- ����

	// �̺�Ʈ �Լ�
	virtual void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void DebugRendering(); // CBuffer <- ����

private:
	bool _first = true;
	int _order = 0;
	std::weak_ptr<GameObject> _owner;
};