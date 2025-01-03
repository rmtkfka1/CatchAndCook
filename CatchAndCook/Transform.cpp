#include "pch.h"
#include "Transform.h"

void Transform::SetDestroy()
{
	Component::SetDestroy();
}

Transform::~Transform()
{
}

void Transform::Init()
{
	Component::Init();
}

void Transform::Start()
{
	Component::Start();
}

void Transform::Update()
{
	Component::Update();
}

void Transform::Update2()
{
	Component::Update2();
}

void Transform::Enable()
{
	Component::Enable();
}

void Transform::Disable()
{
	Component::Disable();
}

void Transform::Destroy()
{
	Component::Destroy();
}

void Transform::RenderBegin()
{
	Component::RenderBegin();
}

void Transform::Rendering()
{
	Component::Rendering();
}

void Transform::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider, other);
}

void Transform::DebugRendering()
{
	Component::DebugRendering();
}
