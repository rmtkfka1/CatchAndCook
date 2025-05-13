#include "pch.h"
#include "Weapon.h"
#include "Transform.h"
#include "Gizmo.h"
Weapon::Weapon()
{
}

Weapon::~Weapon()
{
}

void Weapon::SetDestroy()
{
}

void Weapon::Init()
{
}

void Weapon::Start()
{
}

void Weapon::Update()
{
	if (_isShot)
	{
		if (_moveDist >= _currentWeapon->_range)
		{
			_moveDist = 0.0f;
			_isShot = false;
			_currentWeapon->hook->_transform->SetWorldPosition(_currentWeapon->weaponSlot->_transform->GetWorldPosition());

			cout << _currentWeapon->hook->_transform->GetLocalRotation().x << " " << _currentWeapon->hook->_transform->GetLocalRotation().y << " " << _currentWeapon->hook->_transform->GetLocalRotation().z << endl;
		}

		if (_moveDist < _currentWeapon->_range/2)
		{
			const float speed = _currentWeapon->_speed;
			vec3 currentPos = _currentWeapon->hook->_transform->GetWorldPosition();
			vec3 forward = _currentWeapon->hook->_transform->GetForward();
			_currentWeapon->hook->_transform->SetWorldPosition(currentPos + forward * speed * Time::main->GetDeltaTime());
			_moveDist += speed * Time::main->GetDeltaTime();
		}

		else
		{
			const float speed = _currentWeapon->_speed;
			vec3 slotPos = _currentWeapon->weaponSlot->_transform->GetWorldPosition();
			vec3 currentHookPos = _currentWeapon->hook->_transform->GetWorldPosition();

			vec3 dir = slotPos - currentHookPos;
			dir.Normalize();

			_currentWeapon->hook->_transform->SetWorldPosition(currentHookPos + dir * speed * Time::main->GetDeltaTime());
			_moveDist += speed * Time::main->GetDeltaTime();
		}
		
	}
}

void Weapon::Update2()
{
}

void Weapon::Enable()
{
}

void Weapon::Disable()
{
}

void Weapon::Destroy()
{
}

void Weapon::RenderBegin()
{
}

void Weapon::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Weapon::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

bool Weapon::IsExecuteAble()
{
    return false;
}

void Weapon::SetWeapon(const wstring& weaponName)
{
	if (_currentWeapon && _currentWeapon->GunName == weaponName)
		return;

	auto it = _weapons.find(weaponName);

	if (it == _weapons.end())
	{
		cout << "Weapon Not Found" << endl;
		return;
	}

	if (_currentWeapon)
	{
		_currentWeapon->body->SetActiveSelf(false);
		_currentWeapon->hook->SetActiveSelf(false);
	}

	_currentWeapon = it->second;

	_currentWeapon->body->SetActiveSelf(true);
	_currentWeapon->hook->SetActiveSelf(true);

}

void Weapon::AddWeapon(const wstring& weaponName, const wstring& bodyName, const wstring& hookName, const wstring& weaponSlot)
{
	shared_ptr<Gun> gun = make_shared<Gun>();
	gun->body = SceneManager::main->GetCurrentScene()->Find(bodyName);
	gun->hook = SceneManager::main->GetCurrentScene()->Find(hookName);
	gun->weaponSlot = SceneManager::main->GetCurrentScene()->Find(weaponSlot);

	if (gun->body == nullptr || gun->hook == nullptr || gun->weaponSlot ==nullptr)
	{
		cout << "Weapon Not Found" << endl;
		return;
	}

	gun->GunName = weaponName;
	_weapons[weaponName] = gun;
}




void Weapon::Shot()
{
	_isShot = true;
	_shotForward = _currentWeapon->hook->_transform->GetForward();

}
