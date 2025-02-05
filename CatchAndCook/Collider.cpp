#include "pch.h"
#include "Collider.h"

#include "ColliderManager.h"


Collision::Collision()
{
}

Collision::Collision(CollisionType type)
{
	SetType(type);
}

void Collision::SetBound(BoundingBox box)
{
	_box = box;
}
void Collision::SetBound(BoundingSphere sphere)
{
	_sphere = sphere;
}
void Collision::SetBound(BoundingFrustum frustum)
{
	_frustum = frustum;
}

BoundingOrientedBox Collision::GetWorldBound(std::shared_ptr<Transform> transform)
{
	return BoundingOrientedBox();
}



Collider::~Collider()
{
}

bool Collider::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void Collider::Init()
{
	Component::Init();
}

void Collider::Start()
{
	Component::Start();
	ColliderManager::main->AddCollider(GetCast<Collider>());
}

void Collider::Update()
{
	Component::Update();
}

void Collider::Update2()
{
	Component::Update2();
}

void Collider::Enable()
{
	Component::Enable();
}

void Collider::Disable()
{
	Component::Disable();
}

void Collider::RenderBegin()
{
	Component::RenderBegin();
}

void Collider::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider,other);
	auto& components = GetOwner()->GetComponentAll();
	for(auto& component : components)
		if(component != collider)
			component->CollisionBegin(collider,other);
}

void Collider::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);

	auto& components = GetOwner()->GetComponentAll();
	for(auto& component : components)
		if(component != collider)
			component->CollisionEnd(collider,other);
}

void Collider::SetDestroy()
{
	Component::SetDestroy();
}

void Collider::Destroy()
{
	Component::Destroy();
	ColliderManager::main->RemoveCollider(GetCast<Collider>());
}
