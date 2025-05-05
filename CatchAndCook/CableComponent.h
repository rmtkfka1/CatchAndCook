#pragma once
#include "Component.h"

class GameObject;

class CableComponent : public Component
{

public:
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
	virtual void SetDestroy() override;
	virtual void Destroy();

public:
	void AddWeapon(const wstring& bodyName, const wstring& hookName);
	void ChangeWeapon(const string& name);
	void Shooting();


private:


private:
	unordered_map<wstring, shared_ptr<GameObject>> _bodys;
	unordered_map<wstring, shared_ptr<GameObject>> _hooks;




};

