#include "pch.h"
#include "Component.h"

Component::Component()
{
}

Component::~Component()
{
}

bool Component::operator<(const Component& other) const
{
	return this->_order < other._order;
}

void Component::Init()
{
}

void Component::Start()
{
}

void Component::Update()
{
}

void Component::Update2()
{
}

void Component::Enable()
{
}

void Component::Disable()
{
}

void Component::Destroy()
{
}

void Component::Rendering()
{
}

void Component::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Component::DebugRendering()
{
}
