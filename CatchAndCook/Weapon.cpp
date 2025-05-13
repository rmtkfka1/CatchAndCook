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
		if (_moveDist < _currentWeapon->_range )
		{
			const float speed = _currentWeapon->_speed;
			vec3 currentPos = _currentWeapon->hook->_transform->GetWorldPosition();
			_currentWeapon->hook->_transform->SetWorldPosition(currentPos + _shotForward * speed * Time::main->GetDeltaTime());
			_moveDist += speed * Time::main->GetDeltaTime();
		}
		else
		{
			_isShot = false;
			_currentWeapon->hook->_transform->SetLocalPosition(vec3(0, 0, 0));
			_moveDist = 0.0f;

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

void Weapon::AddWeapon(const wstring& weaponName, const wstring& bodyName, const wstring& hookName )
{
	shared_ptr<Gun> gun = make_shared<Gun>();
	gun->body = SceneManager::main->GetCurrentScene()->Find(bodyName);
	gun->hook = SceneManager::main->GetCurrentScene()->Find(hookName);
	/*auto& firePosObj = SceneManager::main->GetCurrentScene()->Find(FirePoint);*/

	if (gun->body == nullptr || gun->hook == nullptr )
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
