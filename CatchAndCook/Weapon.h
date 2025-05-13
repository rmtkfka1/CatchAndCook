#pragma once
#include "Component.h"

struct Gun
{
	wstring GunName =L"NULL";
	shared_ptr<GameObject> body{};
	shared_ptr<GameObject> hook{};

	vec3 fireLocalPos;

	float _power{};
	float _range =300.0f;
	float _speed =500.0f;
};

class Weapon :public Component
{

public:
	Weapon();
	~Weapon();
	void SetDestroy() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() override;


public:
	void SetWeapon(const wstring& weaponName);
	void AddWeapon(const wstring& weaponName, const wstring& bodyName, const wstring& hookName );
	void Shot();

private:
	bool _isShot = false;
	vec3 _shotForward;
	float _moveDist{};

	shared_ptr<Gun> _currentWeapon;
	unordered_map<wstring, shared_ptr<Gun>> _weapons;

};

