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
        if (_collider->GetType() == CollisionType::Box)
        {
            if (_other->GetType() == CollisionType::Box)
            {
                const auto& myBox = _collider->GetBoundUnion().box;
                const auto& wallBox = _other->GetBoundUnion().box;

                vec3 myCenter = myBox.Center;
                vec3 wallCenter = wallBox.Center;

                vec3 pushDir = myCenter - wallCenter;
                pushDir.Normalize();

                float pushStrength = 50.0 * Time::main->GetDeltaTime();
                vec3 newPos = GetOwner()->_transform->GetLocalPosition() + pushDir * pushStrength;

                GetOwner()->_transform->SetLocalPosition(newPos);

            }
        }
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
	if (other->GetOwner()->HasTag(GameObjectTag::Wall))
	{
		_collider = collider;
		_other = other;
		_onCollision = true;
	} 
}

void WallSlideController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
    if (other->GetOwner()->HasTag(GameObjectTag::Wall))
    {
        _onCollision = false;
    }
}

