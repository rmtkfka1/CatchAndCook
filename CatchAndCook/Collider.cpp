#include "pch.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "GameObject.h"
#include "Transform.h"

Collider::Collider() : _orgin(BoundingOrientedBox()), _bound(BoundingOrientedBox())
{

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

	if(_type == CollisionType::Box)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin._box.Transform(_bound._box,mat);
	}

	else if(_type == CollisionType::Sphere)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin._sphere.Transform(_bound._sphere,mat);
	} 

	else if(_type == CollisionType::Frustum)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin._frustum.Transform(_bound._frustum,mat);
	}
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
	auto a =  GetOwner();
	auto& components = a->GetComponentAll();
	for(auto& component : components)
		if(component.get() != collider.get())
			component->CollisionBegin(collider,other);
}

void Collider::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);

	auto& components = GetOwner()->GetComponentAll();
	for(auto& component : components)
		if(component != GetCast<Component>())
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

bool Collider::CheckCollision(Collider * other)
{
	if(_type == CollisionType::Box)
	{
		if(other->_type == CollisionType::Box)
		{
			return _bound._box.Intersects(other->GetBoundUnion()._box);
		}
		else if(other->_type == CollisionType::Sphere)
		{
			return _bound._box.Intersects(other->GetBoundUnion()._sphere);
		} 
		else if(other->_type == CollisionType::Frustum)
		{
			return _bound._box.Intersects(other->GetBoundUnion()._frustum);
		}


	}
	else if(_type == CollisionType::Sphere)
	{
		if(other->_type == CollisionType::Box)
		{
			return _bound._sphere.Intersects(other->GetBoundUnion()._box);
		}
		else if(other->_type == CollisionType::Sphere)
		{
			return _bound._sphere.Intersects(other->GetBoundUnion()._sphere);
		} 
		else if(other->_type == CollisionType::Frustum)
		{
			return _bound._sphere.Intersects(other->GetBoundUnion()._frustum);
		}


	} 

	else if(_type == CollisionType::Frustum)
	{
		if(other->_type == CollisionType::Box)
		{
			return _bound._frustum.Intersects(other->GetBoundUnion()._box);
		} 
		else if(other->_type == CollisionType::Sphere)
		{
			return _bound._frustum.Intersects(other->GetBoundUnion()._sphere);
		}
		else if(other->_type == CollisionType::Frustum)
		{
			return _bound._frustum.Intersects(other->GetBoundUnion()._frustum);
		}


	}

	return false;
}

void Collider::SetBoundingBox(vec3 center,vec3 extents)
{
	_type = CollisionType::Box;

	_orgin._box = BoundingOrientedBox(center, extents, Quaternion::Identity);
	_bound._box = BoundingOrientedBox(center, extents, Quaternion::Identity);
}

void Collider::SetBoundingSphere(vec3 center,float radius)
{
	_type = CollisionType::Sphere;
	_orgin._sphere = BoundingSphere(center,radius);
	_bound._sphere = BoundingSphere(center,radius);
}

void Collider::SetBoundingFrustum(BoundingFrustum & boundingFrustum)
{
	_type = CollisionType::Frustum;
	_orgin._frustum = boundingFrustum;
	_bound._frustum = boundingFrustum;
}
