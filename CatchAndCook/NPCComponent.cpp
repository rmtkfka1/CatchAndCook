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

	fsm = make_shared<NPCFSMGroup>();
	fsm->Init();
	fsm->AddState(StateType::goto_any, make_shared<NPCGotoAny>());
	fsm->AddState(StateType::idle, make_shared<NPCIdle>());
	fsm->SetNPC(GetCast<NPCComponent>());
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
		auto sit  = animationList->GetAnimations()["sitting"];
		auto sitdown  = animationList->GetAnimations()["sitdown"];
		auto situp  = animationList->GetAnimations()["situp"];
		run->_speedMultiplier = 1.15f;

		sitdown->_isLoop = false;
		situp->_isLoop = false;
		_skinnedHierarchy->Play(idle, 0.25);
	}

	std::vector<std::shared_ptr<GameObject>> pathPointObjects;
	SceneManager::main->GetCurrentScene()->FindsInclude(L"$PathPoint", pathPointObjects);
	for (auto path : pathPointObjects)
		gotoPoints.push_back(path->_transform->GetWorldPosition());

	fsm->ChangeState(StateType::goto_any);
}

void NPCComponent::Update()
{
	Component::Update();
	ColliderManager::main->UpdateDynamicCells();
	fsm->Update();
	fsm->AnyUpdate();
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
	//if ((CameraManager::main->GetActiveCamera()->GetCameraPos() - GetOwner()->_transform->GetWorldPosition()).Length() <= 50)
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



	float gitMargin = 0.25f;//오차 범위
	float upDistance = 1.0f;
	Vector3 upRayOffset = nextPos + Vector3::Up * upDistance;

	RayHit foundGround;
	std::vector<RayHit> hitList;

	if (auto isHit = ColliderManager::main->RayCastAllForMyCellDirect({ upRayOffset, Vector3::Down }, upDistance + gitMargin, hitList,
		BoundingData{ CollisionType::Box, BoundingUnion(BoundingOrientedBox(GetOwner()->_transform->GetWorldPosition(), Vector3(2,2,2), Quaternion::Identity)) }))
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

Vector3 NPCComponent::AdvanceAlongPath(const std::vector<Vector3>& path, const Vector3& worldPos, float t)

{
	size_t n = path.size();
	if (n == 0) return Vector3::Zero;

	// 누적 거리 계산
	std::vector<float> cum(n);
	cum[0] = 0;
	for (size_t i = 1; i < n; ++i)
		cum[i] = cum[i - 1] + (path[i] - path[i - 1]).Length();

	float totalLen = cum[n - 1];

	// 1) P에 가장 가까운 투영점 찾기
	float bestD2 = FLT_MAX;
	float startDist = 0;

	for (size_t i = 0; i + 1 < n; ++i)
	{
		Vector3 A = path[i], B = path[i + 1], AB = B - A;
		float ab2 = AB.Dot(AB);
		float t = (ab2 > 0) ? (worldPos - A).Dot(AB) / ab2 : 0;
		t = std::clamp(t, 0.0f, 1.0f);

		Vector3 Q = A + AB * t;
		float d2 = (Q - worldPos).LengthSquared();

		if (d2 < bestD2)
		{
			bestD2 = d2;
			float segLen = std::sqrt(ab2);
			startDist = cum[i] + t * segLen;
		}
	}

	// 2) 목표 거리 계산 및 클램핑
	float target = startDist + t;
	if (target <= 0)        return path.front();
	if (target >= totalLen) return path.back();

	// 3) 목표 지점이 속한 세그먼트 찾아서 보간
	for (size_t i = 0; i + 1 < n; ++i)
	{
		if (cum[i + 1] >= target)
		{
			float segLen = cum[i + 1] - cum[i];
			float u = (target - cum[i]) / segLen;
			return path[i] + (path[i + 1] - path[i]) * u;
		}
	}

	return path.back(); // 안전 장치
}


// -------- FSM --------

void NPCFSMGroup::AnyUpdate()
{
	StatePatternGroup::AnyUpdate();

}

void NPCFSMGroup::Init()
{
	StatePatternGroup::Init();
}

void NPCFSMGroup::Update()
{
	StatePatternGroup::Update();
}

void NPCFSMGroup::SetNPC(const std::shared_ptr<NPCComponent>& npc)
{
	this->npc = npc;
	for (auto state : statePatterns)
		if (auto state2 = std::dynamic_pointer_cast<NPCState>(state.second))
			state2->SetNPC(npc);
}

// -------- Goto_Any --------

void NPCGotoAny::Init()
{
	StatePattern::Init();
}

void NPCGotoAny::Update()
{
	StatePattern::Update();
	auto npc = this->npc.lock();

	if (NavMeshManager::main->_gizmoDebug)
	{
		Gizmo::Width(0.2f);
		for (int i = 1; i < (int)npc->paths.size(); ++i) {
			Gizmo::Line(npc->paths[i - 1], npc->paths[i], Vector4(1, 0, 0, 1));
		}
		Gizmo::WidthRollBack();
	}

	Vector3 currentWorldPos = npc->GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = npc->AdvanceAlongPath(npc->paths, currentWorldPos, 0.75);
	Vector3 nextDir = Vector2(nextPos.x, nextPos.z) - Vector2(currentWorldPos.x, currentWorldPos.z);
	nextDir.Normalize();
	npc->lookDirection = Vector3(nextDir.x, 0, nextDir.y);

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		auto sit = animationList->GetAnimations()["sitting"];
		auto sitdown = animationList->GetAnimations()["sitdown"];
		auto situp = animationList->GetAnimations()["situp"];

		if ((npc->paths[npc->paths.size() - 1] - currentWorldPos).Length() <= 0.25)
		{
			skinnedHierarchy->Play(idle, 0.25);
			GetGroup()->ChangeState(StateType::idle);
		}
		else
		{
			skinnedHierarchy->Play(walk, 0.25);
		}
	}
}

void NPCGotoAny::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	StatePattern::Begin(type, prevState);

	auto npc = this->npc.lock();
	npc->paths = NavMeshManager::main->CalculatePath(npc->GetOwner()->_transform->GetWorldPosition(), npc->gotoPoints[_random_dist(_random) % npc->gotoPoints.size()]);

	auto animationList = npc->GetOwner()->GetComponentWithChilds<AnimationListComponent>();
	auto skinnedHierarchy = npc->GetOwner()->GetComponentWithChilds<SkinnedHierarchy>();

	if (animationList)
	{
		auto walk = animationList->GetAnimations()["walk"];
		auto idle = animationList->GetAnimations()["idle"];
		auto run = animationList->GetAnimations()["run"];
		skinnedHierarchy->Play(walk, 0.25);
	}
}

bool NPCGotoAny::TriggerUpdate()
{
	return StatePattern::TriggerUpdate();
}

void NPCGotoAny::End(const std::shared_ptr<StatePattern>& nextState)
{
	StatePattern::End(nextState);
}


// -------- IDLE --------

void NPCIdle::Init()
{
	NPCState::Init();
}

void NPCIdle::Update()
{
	NPCState::Update();
}

void NPCIdle::Begin(StateType type, const std::shared_ptr<StatePattern>& prevState)
{
	NPCState::Begin(type, prevState);
	GetGroup()->ChangeState(StateType::goto_any);
}

bool NPCIdle::TriggerUpdate()
{
	return NPCState::TriggerUpdate();
}

void NPCIdle::End(const std::shared_ptr<StatePattern>& nextState)
{
	NPCState::End(nextState);
}
