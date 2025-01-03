#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	_component.reserve(4);
}

GameObject::GameObject(std::wstring name)
{
	this->SetName(name);
}

GameObject::~GameObject()
{
	_component.clear();
}


void GameObject::Init()
{
	std::shared_ptr<GameObject> gameObject = GetCast<GameObject>();

	
}

void GameObject::Start()
{

}

void GameObject::Update()
{
}

void GameObject::Update2()
{
}

void GameObject::Enable()
{
}

void GameObject::Disable()
{
}

void GameObject::Destroy()
{
}

void GameObject::RenderBegin()
{
}

void GameObject::Rendering()
{
}

void GameObject::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void GameObject::DebugRendering()
{
}
