﻿#include "pch.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "GameObject.h"
#include "Gizmo.h"
#include "PhysicsComponent.h"
#include "simple_mesh_ext.h"
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

	groupId = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	if (auto obj = GetOwner()->GetComponentWithParents<PhysicsComponent>()) {
		groupRootObject = obj->GetOwner();
	}

	//if (GetOwner()->GetType() == GameObjectType::Static && GetOwner()->GetActive())
	//{
	//	CalculateBounding();
	//	ColliderManager::main->AddCollider(GetCast<Collider>());
	//}
	CalculateBounding();
	ColliderManager::main->AddCollider(GetCast<Collider>());
}

void Collider::Update()
{
	Component::Update();

	if (GetOwner()->GetType() == GameObjectType::Dynamic)
	{
		CalculateBounding();
		ColliderManager::main->AddCollider(GetCast<Collider>());
	}
}

void Collider::Update2()
{
	Component::Update2();

	CalculateBounding();
}

void Collider::Enable()
{
	Component::Enable();

	ColliderManager::main->AddColliderForRay(GetCast<Collider>());

	if (GetOwner()->GetType() == GameObjectType::Static)
		CalculateBounding();
	

	groupId = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	if (auto obj = GetOwner()->GetComponentWithParents<PhysicsComponent>()) {
		groupRootObject = obj->GetOwner();
	}
}

void Collider::Disable()
{
	Component::Disable();

	ColliderManager::main->RemoveCollider(GetCast<Collider>());
	ColliderManager::main->RemoveAColliderForRay(GetCast<Collider>());
}

void Collider::RenderBegin()
{
	Component::RenderBegin();

	bool isCollision = ColliderManager::main->IsCollision(GetCast<Collider>());

	if (HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::Collision))
	{

		if (_type == CollisionType::Box)
		{
			Gizmo::Width(0.1f);
			Gizmo::Box(_bound.box, !isCollision ? Vector4(0, 1, 0, 1) : Vector4(1, 0.5, 0, 1));
			Gizmo::WidthRollBack();
		}

		else if (_type == CollisionType::Sphere)
		{
			Gizmo::Width(0.1f);
			Gizmo::Sphere(_bound.sphere, !isCollision ? Vector4(0, 1, 0, 1) : Vector4(1, 0.5, 0, 1));
			Gizmo::WidthRollBack();
		}

		else if (_type == CollisionType::Frustum)
		{
			Gizmo::Width(0.1f);
			Gizmo::Frustum(_bound.frustum, !isCollision ? Vector4(0, 1, 0, 1) : Vector4(1, 0.5, 0, 1));
			Gizmo::WidthRollBack();
		}
	}
}

void Collider::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);

}

void Collider::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void Collider::SetDestroy()
{
	Component::SetDestroy();
}

void Collider::Destroy()
{
	Component::Destroy();
}

vec3 Collider::GetCenter()
{
	if (_type == CollisionType::Box)
		return _bound.box.Center;
	else if (_type == CollisionType::Sphere)
		return _bound.sphere.Center;
	else if (_type == CollisionType::Frustum)
		return _bound.frustum.Origin;
}

bool Collider::CheckCollision(const std::shared_ptr<Collider>& other)
{
	if (_type == CollisionType::Box)
	{
		if (other->_type == CollisionType::Box)
		{
			return _bound.box.Intersects(other->_bound.box);
		}
		else if (other->_type == CollisionType::Sphere)
		{
			return _bound.box.Intersects(other->_bound.sphere);
		}
		else if (other->_type == CollisionType::Frustum)
		{
			return _bound.box.Intersects(other->_bound.frustum);
		}

	}
	else if (_type == CollisionType::Sphere)
	{
		if (other->_type == CollisionType::Box)
		{
			return _bound.sphere.Intersects(other->_bound.box);
		}
		else if (other->_type == CollisionType::Sphere)
		{
			return _bound.sphere.Intersects(other->_bound.sphere);
		}
		else if (other->_type == CollisionType::Frustum)
		{
			return _bound.sphere.Intersects(other->_bound.frustum);
		}
	}

	else if (_type == CollisionType::Frustum)
	{
		if (other->_type == CollisionType::Box)
		{
			return _bound.frustum.Intersects(other->_bound.box);
		}
		else if (other->_type == CollisionType::Sphere)
		{
			return _bound.frustum.Intersects(other->_bound.sphere);
		}
		else if (other->_type == CollisionType::Frustum)
		{
			return _bound.frustum.Intersects(other->_bound.frustum);
		}


	}

	return false;
}

bool Collider::CheckCollision(const CollisionType& type, const BoundingUnion& bound)
{
	if (_type == CollisionType::Box)
	{
		if (type == CollisionType::Box)
		{
			return _bound.box.Intersects(bound.box);
		}
		else if (type == CollisionType::Sphere)
		{
			return _bound.box.Intersects(bound.sphere);
		}
		else if (type == CollisionType::Frustum)
		{
			return _bound.box.Intersects(bound.frustum);
		}

	}
	else if (_type == CollisionType::Sphere)
	{
		if (type == CollisionType::Box)
		{
			return _bound.sphere.Intersects(bound.box);
		}
		else if (type == CollisionType::Sphere)
		{
			return _bound.sphere.Intersects(bound.sphere);
		}
		else if (type == CollisionType::Frustum)
		{
			return _bound.sphere.Intersects(bound.frustum);
		}
	}

	else if (_type == CollisionType::Frustum)
	{
		if (type == CollisionType::Box)
		{
			return _bound.frustum.Intersects(bound.box);
		}
		else if (type == CollisionType::Sphere)
		{
			return _bound.frustum.Intersects(bound.sphere);
		}
		else if (type == CollisionType::Frustum)
		{
			return _bound.frustum.Intersects(bound.frustum);
		}


	}

	return false;
}

bool Collider::RayCast(const Ray& ray, const float& dis, RayHit& hit)
{
	hit.distance = dis;
	hit.collider = this;
	hit.gameObject = GetOwner().get();

	////// _bound.box를 지역 변수로 캐싱하여 반복 접근을 줄입니다.

	if (_type == CollisionType::Box)
	{
		const auto& box = _bound.box;
		// Ray와 박스 간의 충돌 여부를 테스트합니다.
		if (ray.Intersects(box, hit.distance))
		{
			// 월드 좌표계에서 충돌 지점을 계산합니다.
			hit.worldPos = ray.position + ray.direction * hit.distance;
			// 박스의 로컬 좌표계로 충돌 지점을 변환합니다.
			// 로컬 좌표계에서는 박스가 축 정렬되어 있으므로 면들은 x, y, z 축에 평행합니다.
			const Quaternion orientation = box.Orientation;
			Quaternion invOrientation;
			orientation.Inverse(invOrientation);
			const Vector3 localHitPoint = Vector3::Transform(hit.worldPos - box.Center, invOrientation);

			// Determine on which face the hit occurred.
			// Since the box is axis-aligned in local space, one coordinate will be nearly equal to the corresponding extent.
			const float diffX = fabsf(fabs(localHitPoint.x) - box.Extents.x);
			const float diffY = fabsf(fabs(localHitPoint.y) - box.Extents.y);
			const float diffZ = fabsf(fabs(localHitPoint.z) - box.Extents.z);

			// 세 축 중 차이가 가장 작은 축이 충돌 면임을 판단합니다.
			if (diffX < diffY && diffX < diffZ)
			{
				// X면에서 충돌이 발생함 (로컬 X 좌표의 부호에 따라 결정)
				hit.normal = Vector3((localHitPoint.x > 0.0f) ? 1.0f : -1.0f, 0.0f, 0.0f);
			}
			else if (diffY < diffZ)
			{
				// Y면에서 충돌이 발생함
				hit.normal = Vector3(0.0f, (localHitPoint.y > 0.0f) ? 1.0f : -1.0f, 0.0f);
			}
			else
			{
				// Z면에서 충돌이 발생함
				hit.normal = Vector3(0.0f, 0.0f, (localHitPoint.z > 0.0f) ? 1.0f : -1.0f);
			}

			// 로컬 좌표계의 법선을 월드 좌표계로 변환합니다.
			hit.normal = Vector3::Transform(hit.normal, orientation);
			hit.normal.Normalize();
			hit.isHit = true;
			return true;
		}
	}
	if (_type == CollisionType::Sphere)
	{
		const auto& sphere = _bound.sphere;
		// Ray와 구(BoundingSphere) 간의 충돌 여부를 테스트합니다.
		if (ray.Intersects(sphere, hit.distance))
		{
			// 월드 좌표계에서 충돌 지점을 계산합니다.
			hit.worldPos = ray.position + ray.direction * hit.distance;
			// 구의 경우, 충돌한 표면의 법선은 (충돌 지점 - 구 중심)을 정규화한 값입니다.
			hit.normal = hit.worldPos - sphere.Center;
			hit.normal.Normalize();
			hit.isHit = true;
			return true;
		}
	}

	hit.isHit = false;
	// No intersection occurred.
	return false;
}

void Collider::SetBoundingBox(vec3 center, vec3 extents)
{
	_type = CollisionType::Box;

	_orgin.box = BoundingOrientedBox(center, extents, Quaternion::Identity);
	_bound.box = BoundingOrientedBox(center, extents, Quaternion::Identity);

}

void Collider::SetBoundingSphere(vec3 center, float radius)
{
	_type = CollisionType::Sphere;
	_orgin.sphere = BoundingSphere(center, radius);
	_bound.sphere = BoundingSphere(center, radius);




}

void Collider::SetBoundingFrustum(BoundingFrustum& boundingFrustum)
{
	_type = CollisionType::Frustum;
	_orgin.frustum = boundingFrustum;
	_bound.frustum = boundingFrustum;
}

void Collider::CalculateBounding()
{
	if (_type == CollisionType::Box)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);

		if (onwerTransform->IsLocalToWorldChanged())
		{
			_bound.box = TransformBoundingOrientedBox(_orgin.box, mat, onwerTransform->GetWorldScale(), onwerTransform->GetWorldRotation());
		}
	}

	else if (_type == CollisionType::Sphere)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin.sphere.Transform(_bound.sphere, mat);
	}

	else if (_type == CollisionType::Frustum)
	{
		auto onwerTransform = GetOwner()->_transform;
		Matrix mat;
		onwerTransform->GetLocalToWorldMatrix_BottomUp(mat);
		_orgin.frustum.Transform(_bound.frustum, mat);
	}
}

pair<vec3, vec3> Collider::GetMinMax()
{
	if (_type == CollisionType::Box)
	{
		Matrix rotMatrix = Matrix::CreateFromQuaternion(_bound.box.Orientation);
		vec3 center = _bound.box.Center;
		vec3 extents = _bound.box.Extents;

		vec3 localVertices[8] = {
			vec3(-extents.x, -extents.y, -extents.z),
			vec3(-extents.x, -extents.y,  extents.z),
			vec3(-extents.x,  extents.y, -extents.z),
			vec3(-extents.x,  extents.y,  extents.z),
			vec3(extents.x, -extents.y, -extents.z),
			vec3(extents.x, -extents.y,  extents.z),
			vec3(extents.x,  extents.y, -extents.z),
			vec3(extents.x,  extents.y,  extents.z)
		};

		vec3 worldMin = vec3(FLT_MAX, FLT_MAX, FLT_MAX);
		vec3 worldMax = vec3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for (int i = 0; i < 8; i++)
		{
			vec3 worldVertex = center + vec3(
				localVertices[i].x * rotMatrix.m[0][0] + localVertices[i].y * rotMatrix.m[1][0] + localVertices[i].z * rotMatrix.m[2][0],
				localVertices[i].x * rotMatrix.m[0][1] + localVertices[i].y * rotMatrix.m[1][1] + localVertices[i].z * rotMatrix.m[2][1],
				localVertices[i].x * rotMatrix.m[0][2] + localVertices[i].y * rotMatrix.m[1][2] + localVertices[i].z * rotMatrix.m[2][2]
			);

			worldMin = vec3(
				std::min(worldMin.x, worldVertex.x),
				std::min(worldMin.y, worldVertex.y),
				std::min(worldMin.z, worldVertex.z)
			);

			worldMax = vec3(
				std::max(worldMax.x, worldVertex.x),
				std::max(worldMax.y, worldVertex.y),
				std::max(worldMax.z, worldVertex.z)
			);
		}


		return std::make_pair(worldMin, worldMax);
	}

	else if (_type == CollisionType::Sphere)
	{
		return std::make_pair(_bound.sphere.Center - vec3(_bound.sphere.Radius), _bound.sphere.Center + vec3(_bound.sphere.Radius));
	}

	else if (_type == CollisionType::Frustum)
	{
		return std::make_pair(vec3(0, 0, 0), vec3(0, 0, 0));
	}

	return std::make_pair(vec3(0, 0, 0), vec3(0, 0, 0));
}

Vector3 Collider::GetBoundCenter()
{
	if (_type == CollisionType::Box)
	{
		return _bound.box.Center;
	}
	if (_type == CollisionType::Sphere)
	{
		return _bound.sphere.Center;
	}
	return Vector3::Zero;
}
