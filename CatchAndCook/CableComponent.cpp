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
	cout << "호출1" << endl;
	/*auto it = _hooks.find(L"hook");*/
	auto it = SceneManager::main->GetCurrentScene()->Find(L"hook");

	wcout << it->GetName() << endl;

	//vec3 forward = it->_transform->GetForward();
	//vec3 currentPos = it->_transform->GetWorldPosition();
	//it->_transform->SetWorldPosition(
	//	currentPos + 30.0f * forward * Time::main->GetDeltaTime()
	//);
	
}

void CableComponent::AddWeapon(const wstring& bodyName, const wstring& hookName) 
{

	auto body = SceneManager::main->GetCurrentScene()->Find(bodyName);

	if (body)
	{
		wcout << body->GetName() << "찾음" << endl;

	
		_bodys[bodyName] = body;
		

	}

	auto hook = SceneManager::main->GetCurrentScene()->Find(hookName);

	if (hook)
	{
		wcout << hook->GetName() << "찾음" << endl;
		_hooks[hookName] = hook;
		
	}

}
