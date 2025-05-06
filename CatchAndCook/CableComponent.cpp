#include "pch.h"
#include "CableComponent.h"
#include "GameObject.h"
#include "Transform.h"
void CableComponent::Init()
{


}

void CableComponent::Start()
{

}

void CableComponent::Update()
{
}

void CableComponent::Update2()
{
}

void CableComponent::Enable()
{
}

void CableComponent::Disable()
{
}

void CableComponent::RenderBegin()
{
}

void CableComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void CableComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void CableComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void CableComponent::SetDestroy()
{
}

void CableComponent::Destroy()
{
}

void CableComponent::ChangeWeapon(const string& name)
{



}

void CableComponent::Shooting()
{
	if (_currentHook)
	{
		wcout << _currentHook->GetName() << endl;
		vec3 forward = _currentHook->_transform->GetForward();
		vec3 worldPos = _currentHook->_transform->GetWorldPosition();
		_currentHook->_transform->SetWorldPosition(worldPos + forward * 30.0f * Time::main->GetDeltaTime());
	}
	
}

void CableComponent::AddWeapon(const wstring& bodyName, const wstring& hookName) 
{

	auto body = SceneManager::main->GetCurrentScene()->Find(bodyName);

	if (body)
	{
		wcout << body->GetName() << "찾음" << endl;

		if (_bodys.find(bodyName) == _bodys.end())
		{
			_bodys[bodyName] = body;
			_currentBody = body;
		}
	}

	auto hook = SceneManager::main->GetCurrentScene()->Find(hookName);

	if (hook)
	{
		wcout << hook->GetName() << "찾음" << endl;
		if (_hooks.find(bodyName) == _hooks.end())
		{
			_hooks[hookName] = hook;
			_currentHook = hook;
		}
		
	}



}
