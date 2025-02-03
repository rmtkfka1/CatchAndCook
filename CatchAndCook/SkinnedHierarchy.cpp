#include "pch.h"
#include "SkinnedHierarchy.h"

SkinnedHierarchy::~SkinnedHierarchy()
{
}

bool SkinnedHierarchy::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SkinnedHierarchy::Init()
{
	Component::Init();
}

void SkinnedHierarchy::Start()
{
	Component::Start();
}

void SkinnedHierarchy::Update()
{
	Component::Update();
}

void SkinnedHierarchy::Update2()
{
	Component::Update2();
}

void SkinnedHierarchy::Enable()
{
	Component::Enable();
}

void SkinnedHierarchy::Disable()
{
	Component::Disable();
}

void SkinnedHierarchy::RenderBegin()
{
	Component::RenderBegin();
}

void SkinnedHierarchy::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider,other);
}

void SkinnedHierarchy::SetDestroy()
{
	Component::SetDestroy();
}

void SkinnedHierarchy::Destroy()
{
	Component::Destroy();
}
