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
		_orgin.box.Transform(_bound.box,mat);
	}

	else if(_type == CollisionType::Sphere)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin.sphere.Transform(_bound.sphere,mat);
	} 

	else if(_type == CollisionType::Frustum)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin.frustum.Transform(_bound.frustum,mat);
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

}

void Collider::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);
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
			return _bound.box.Intersects(other->GetBoundUnion().box);
		}
		else if(other->_type == CollisionType::Sphere)
		{
			return _bound.box.Intersects(other->GetBoundUnion().sphere);
		} 
		else if(other->_type == CollisionType::Frustum)
		{
			return _bound.box.Intersects(other->GetBoundUnion().frustum);
		}


	}
	else if(_type == CollisionType::Sphere)
	{
		if(other->_type == CollisionType::Box)
		{
			return _bound.sphere.Intersects(other->GetBoundUnion().box);
		}
		else if(other->_type == CollisionType::Sphere)
		{
			return _bound.sphere.Intersects(other->GetBoundUnion().sphere);
		} 
		else if(other->_type == CollisionType::Frustum)
		{
			return _bound.sphere.Intersects(other->GetBoundUnion().frustum);
		}


	} 

	else if(_type == CollisionType::Frustum)
	{
		if(other->_type == CollisionType::Box)
		{
			return _bound.frustum.Intersects(other->GetBoundUnion().box);
		} 
		else if(other->_type == CollisionType::Sphere)
		{
			return _bound.frustum.Intersects(other->GetBoundUnion().sphere);
		}
		else if(other->_type == CollisionType::Frustum)
		{
			return _bound.frustum.Intersects(other->GetBoundUnion().frustum);
		}


	}

	return false;
}

void Collider::SetBoundingBox(vec3 center,vec3 extents)
{
	_type = CollisionType::Box;

	_orgin.box = BoundingOrientedBox(center, extents, Quaternion::Identity);
	_bound.box = BoundingOrientedBox(center, extents, Quaternion::Identity);
}

void Collider::SetBoundingSphere(vec3 center,float radius)
{
	_type = CollisionType::Sphere;
	_orgin.sphere = BoundingSphere(center,radius);
	_bound.sphere = BoundingSphere(center,radius);
}

void Collider::SetBoundingFrustum(BoundingFrustum & boundingFrustum)
{
	_type = CollisionType::Frustum;
	_orgin.frustum = boundingFrustum;
	_bound.frustum = boundingFrustum;
}
