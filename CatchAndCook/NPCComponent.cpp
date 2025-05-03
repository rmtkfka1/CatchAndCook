#include "pch.h"
#include "NPCComponent.h"

#include "PlayerController.h"

#include "Animation.h"
#include "Camera.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "Collider.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "PhysicsComponent.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "Transform.h"
#include "AnimationListComponent.h"

COMPONENT(NPCComponent)

NPCComponent::~NPCComponent()
{

}

bool NPCComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void NPCComponent::Init()
{
	Component::Init();
}

void NPCComponent::Start()
{
	Component::Start();

	auto _animationList = GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto _skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (auto skinnedHierarchy = _skinnedHierarchy)
	{
		skinnedHierarchy->_speedMultiple = 1.2f;
	}
	if (auto animationList = _animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		run->_speedMultiplier = 1.15f;

		_skinnedHierarchy->Play(walk, 0.25);
	}
}

void NPCComponent::Update()
{
	Component::Update();

	auto skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();
	auto velocity = skinnedHierarchy->GetDeltaPosition() * GetOwner()->_transform->GetWorldScale().y;
	std::cout << to_string(velocity) << "\n";
	GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + velocity);
}

void NPCComponent::Update2()
{
	Component::Update2();
}

void NPCComponent::Enable()
{
	Component::Enable();
}

void NPCComponent::Disable()
{
	Component::Disable();
}

void NPCComponent::RenderBegin()
{
	Component::RenderBegin();
}

void NPCComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void NPCComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void NPCComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void NPCComponent::SetDestroy()
{
	Component::SetDestroy();
}

void NPCComponent::Destroy()
{
	Component::Destroy();
}
