#include "pch.h"
#include "WallSlideController.h"
#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"

WallSlideController::WallSlideController()
{
}

WallSlideController::~WallSlideController()
{
}

void WallSlideController::Init()
{
}

void WallSlideController::Start()
{
}

void WallSlideController::Update()
{
	if (_onCollision)
	{
		auto& transform =  GetOwner()->_transform;

		auto& mybox = _collider->GetBoundUnion().box;
		auto& otherbox = _other->GetBoundUnion().box;

		auto direction = mybox.Center - otherbox.Center;
		direction.Normalize();

		auto pos = transform->GetLocalPosition();
		pos += direction * 50.0f * Time::main->GetDeltaTime();
		transform->SetLocalPosition(pos);
	}
}

void WallSlideController::Update2()
{
}

void WallSlideController::Enable()
{
}

void WallSlideController::Disable()
{
}

void WallSlideController::RenderBegin()
{
}

void WallSlideController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

	if (HasTag(GetOwner()->GetTag(), GameObjectTag::Wall))
	{
		_collider = collider;
		_other = other;
		_onCollision = true;
	}
}

void WallSlideController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	if (HasTag(GetOwner()->GetTag(), GameObjectTag::Wall))
	{
		_onCollision = false;
	}
}
