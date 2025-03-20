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
		if (other->GetType() == CollisionType::Box)
		{
			cout << "안녕" << endl;
		}
	
	}
}

void WallSlideController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}
