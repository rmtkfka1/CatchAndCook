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
#include "NavMeshManager.h"

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

	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);
	for (auto path : pathPointObjects)
		gotoPoints.push_back(path->_transform->GetWorldPosition());

}

void NPCComponent::Update()
{
	Component::Update();
	if (type == NPCStateType::stay) {
		paths = NavMeshManager::main->CalculatePath(GetOwner()->_transform->GetWorldPosition(), gotoPoints[_random_dist(_random) % gotoPoints.size()]);
		type = NPCStateType::goto_any;
	}

	Gizmo::Width(0.2f);
	for (int i = 1; i < (int)paths.size(); ++i) {
		Gizmo::Line(
			paths[i - 1], paths[i], Vector4(1, 0, 0, 1));
	}
	Gizmo::WidthRollBack();

	//auto skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();
	//velocity = skinnedHierarchy->GetDeltaPosition() * GetOwner()->_transform->GetWorldScale().y;
	//GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + velocity);
	//velocity
	
}

void NPCComponent::MoveControl()
{
	auto _skinnedHierarchy = GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	Vector3 prevVelocity = velocity;
	if (auto skinnedHierarchy = _skinnedHierarchy)
		velocity = skinnedHierarchy->GetDeltaPosition() * GetOwner()->_transform->GetWorldScale().y;
	velocity = Vector3(velocity.x, prevVelocity.y, velocity.z);

	
	GetOwner()->_transform->LookUp(lookDirection, Vector3::Up);


	isGround = false;

	std::vector<BoundingData> playerColliderDatas;
	std::vector<std::shared_ptr<Collider>> playerColliders;

	GetOwner()->GetComponentsWithChilds<Collider>(playerColliders);
	int playerGroupID = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	for (auto& collider : playerColliders) {
		if (playerGroupID == collider->GetGroupID()) {
			collider->CalculateBounding();
			playerColliderDatas.push_back(collider->GetBoundingData());
		}
	}


	Vector3 currentPos = GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = currentPos;

	Vector3 velocityDirectionXZ = velocity; //  * Time::main->GetDeltaTime()
	velocityDirectionXZ.y = 0;

	//if (velocityDirectionXZ != Vector3::Zero)
	if ((CameraManager::main->GetActiveCamera()->GetCameraPos() - GetOwner()->_transform->GetWorldPosition()).Length() <= 20)
	{
		for (auto boundingData : playerColliderDatas)
		{
			// 이동할 위치 미리 계산
			Vector3 nextCenter;
			float currentRadius;

			currentRadius = boundingData.GetRadius();
			nextCenter = boundingData.SetCenter(boundingData.GetCenter() + velocityDirectionXZ);


			// 이동할 곳에서 충돌되는지 보기.
			std::vector<std::shared_ptr<Collider>> otherColliders;
			if (ColliderManager::main->CollisionChecksDirect(boundingData, otherColliders))
			{
				for (auto& otherCollider : otherColliders)
				{
					if (otherCollider->GetGroupID() != playerGroupID)
					{
						auto otherBoundingData = otherCollider->GetBoundingData();
						Vector3 hitPosition = Collider::GetContactPoint(boundingData, otherBoundingData);

						auto pushDir = nextCenter - hitPosition;
						auto pushNormal = pushDir;
						pushNormal.Normalize();

						velocityDirectionXZ += pushNormal * std::max(currentRadius - pushDir.Length(), 0.0f);
					}
				}
			}
		}
	}
	float velocityDirectionY = velocity.y * Time::main->GetDeltaTime() * 60;
	nextPos += Vector3(velocityDirectionXZ.x, velocityDirectionY, velocityDirectionXZ.z); // velocityDirectionXZ.y + velocityDirectionY



	float gitMargin = 0.15f;//오차 범위
	float upDistance = 1.0f;
	Vector3 upRayOffset = nextPos + Vector3::Up * upDistance;

	RayHit foundGround;
	std::vector<RayHit> hitList;

	if (auto isHit = ColliderManager::main->RayCastAllForMyCell({ upRayOffset, Vector3::Down }, upDistance + gitMargin, hitList, GetOwner()))
	{
		for (auto& hit : hitList)
		{
			if (hit.gameObject->GetRoot() != GetOwner()->GetRoot())
			{
				if (hit.distance <= upDistance + gitMargin) {
					foundGround = hit;
					break;
				}
			}
		}
	}
	if (foundGround)
	{
		isGround = true;
		nextPos.y = upRayOffset.y - foundGround.distance; //upRayOffset.y -
	}

	if (isGround)
	{
		velocity.y = 0;
	}
	else
	{
		velocity.y -= 9.81 * 0.5 * Time::main->GetDeltaTime();
	}

	// ------------- 공통 로직 ------------- 

	GetOwner()->_transform->SetWorldPosition(nextPos);
}

void NPCComponent::Update2()
{
	Component::Update2();
	MoveControl();
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
