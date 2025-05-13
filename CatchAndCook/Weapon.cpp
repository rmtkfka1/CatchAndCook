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
	switch (_state)
	{
	case Idle:
	{

	}
		break;
	case Shot:
		{
		if (_moveDist < _currentWeapon->_range / 2)
		{
			const float speed = _currentWeapon->_speed;
			vec3 currentPos = _currentWeapon->hook->_transform->GetWorldPosition();
			vec3 forward = _currentWeapon->hook->_transform->GetForward();
			_currentWeapon->hook->_transform->SetWorldPosition(currentPos + forward * speed * Time::main->GetDeltaTime());
			_moveDist += speed * Time::main->GetDeltaTime();
		}
		else
		{
			_state = Reload;
		}
	}
		break;
	case Reload:
	{
		const float speed = _currentWeapon->_speed;
		vec3 slotPos = _currentWeapon->weaponSlot->_transform->GetWorldPosition();
		vec3 currentHookPos = _currentWeapon->hook->_transform->GetWorldPosition();

		vec3 dir = slotPos - currentHookPos;
		dir.Normalize();

		_currentWeapon->hook->_transform->SetWorldPosition(currentHookPos + dir * speed * Time::main->GetDeltaTime());
		_moveDist += speed * Time::main->GetDeltaTime();

		if (_moveDist > _currentWeapon->_range)
		{
			_currentWeapon->hook->_transform->SetLocalPosition(vec3(0, 0, 0));
			_state = Idle;
			_moveDist = 0;
		}
	}
		break;
	default:
		break;
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

void Weapon::ChangeState(const WeaponState& state)
{
	if (_state == state)
		return;

	_state = state;

	switch (_state)
	{
	case Idle:
		break;
	case Shot:
		break;
	case Reload:
		break;
	default:
		break;
	}
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




