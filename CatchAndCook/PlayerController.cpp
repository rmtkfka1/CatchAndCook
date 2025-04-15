#include "pch.h"
#include "PlayerController.h"

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


COMPONENT(PlayerController)

PlayerController::~PlayerController()
{

}

bool PlayerController::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void PlayerController::Init()
{
	Component::Init();

}

void PlayerController::Start()
{
	Component::Start();
	camera = FindObjectByType<CameraComponent>();
}

void PlayerController::Update()
{
	Component::Update();
	ColliderManager::main->UpdateDynamicCells();
	CameraControl();
	MoveControl();

	//GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Forward * 10.0f * Time::main->GetDeltaTime());
}



void PlayerController::CameraControl()
{
	auto cameraTransform = camera.lock()->GetOwner()->_transform;
	auto currentMousePosition = Input::main->GetMousePosition();

	_targetOffset = GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 1.1f;

	_currentOffset = Vector3::Lerp(_currentOffset, _targetOffset,
		(float)Time::main->GetDeltaTime() * controlInfo.cameraMoveSmooth
		* std::min(Vector3::Distance(_currentOffset, _targetOffset), 1.0f));

	// 카메라 방향 계산식
	Vector2 mouseDelta = currentMousePosition - Input::main->GetMousePrevPosition();
	Vector3 deltaEuler = Vector3(mouseDelta.y, mouseDelta.x, 0) * 0.01f * 0.5f;
	Vector3 calculateEuler = deltaEuler + _targetEuler;

	calculateEuler.z = 0.0f;
	if (calculateEuler.x * R2D <= -80) calculateEuler.x = -80 * D2R;
	if (calculateEuler.x * R2D >= 85) calculateEuler.x = 85 * D2R;

	_targetEuler = calculateEuler;
	_currentEuler = Vector3::Lerp(_currentEuler, _targetEuler, Time::main->GetDeltaTime() * controlInfo.cameraRotationSmooth);


	Vector3 cameraNextPosition = _currentOffset;
	Quaternion finalRotation = Quaternion::CreateFromYawPitchRoll(_currentEuler);
	Vector3 _currentNextDirection = Vector3::Transform(Vector3::Forward, finalRotation);


	finalRotation = Quaternion::CreateFromYawPitchRoll(_currentEuler);
	_currentNextDirection = Vector3::Transform(Vector3::Forward, finalRotation);

	float distance = 3.6;

	std::vector<RayHit> hitList;
	if (auto isHit = ColliderManager::main->RayCastAll({ _currentOffset, -_currentNextDirection }, distance * 2, hitList))
		for (auto& hit : hitList)
			if (hit.gameObject->GetRoot() != GetOwner()->GetRoot())
			{
				distance = std::min(hit.distance, distance) - 0.05f;
				break;
			}

	cameraNextPosition = _currentOffset - _currentNextDirection * distance;

	cameraTransform->SetWorldPosition(cameraNextPosition);
	cameraTransform->LookUp(_currentNextDirection, Vector3::Up);
}

void PlayerController::MoveControl()
{
	Quaternion lookRotation = Quaternion::Identity;
	lookRotation = Quaternion::CreateFromYawPitchRoll(Vector3(0, _targetEuler.y, 0));

	// ------------- 공통 로직 ------------- 
	Vector3 moveDirection = Vector3(
		(Input::main->GetKey(KeyCode::D) ? 1 : 0) - (Input::main->GetKey(KeyCode::A) ? 1 : 0), 0,
		(Input::main->GetKey(KeyCode::W) ? 1 : 0) - (Input::main->GetKey(KeyCode::S) ? 1 : 0));
	moveDirection.Normalize();
	targetLookWorldDirection = Vector3::Transform(moveDirection, lookRotation);

	if (targetLookWorldDirection != Vector3::Zero)
	{
		currentLookWorldRotation = Quaternion::Slerp(currentLookWorldRotation, Quaternion::LookRotation(targetLookWorldDirection, Vector3::Transform(Vector3::Up, lookRotation)),
			Time::main->GetDeltaTime() * 30);

		// 등속 운동 로직
		Vector3 prevVelocity = velocity;
		prevVelocity += targetLookWorldDirection * controlInfo.moveForce * Time::main->GetDeltaTime() * 60;
		prevVelocity.y = 0;
		if (prevVelocity.Length() > controlInfo.maxSpeed) {
			prevVelocity.Normalize();
			prevVelocity *= controlInfo.maxSpeed;
		}

		// 이동
		velocity = Vector3(prevVelocity.x, velocity.y, prevVelocity.z);
	}
	GetOwner()->_transform->SetWorldRotation(currentLookWorldRotation);

	

	std::vector<std::pair<CollisionType, BoundingUnion>> playerColliderDatas;
	std::vector<std::shared_ptr<Collider>> playerColliders;

	GetOwner()->GetComponentsWithChilds<Collider>(playerColliders);
	int playerGroupID = PhysicsComponent::GetPhysicsGroupID(GetOwner());
	for (auto& collider : playerColliders) {
		if (playerGroupID == collider->GetGroupID()) {
			collider->CalculateBounding();
			playerColliderDatas.push_back(std::make_pair(collider->GetType(), collider->GetBoundUnion()));
		}
	}


	Vector3 currentPos = GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = currentPos;

	Vector3 velocityDirectionXZ = velocity * Time::main->GetDeltaTime();
	velocityDirectionXZ.y = 0;

	//if (velocityDirectionXZ != Vector3::Zero)
	{
		for (auto [type, bound] : playerColliderDatas)
		{
			// 이동할 위치 미리 계산
			Vector3 currentCenter;
			float currentRadius;
			if (type == CollisionType::Box)
			{
				currentCenter = bound.box.Center = bound.box.Center + velocityDirectionXZ;
				currentRadius = Vector3(bound.box.Extents).Length();
			}
			if (type == CollisionType::Sphere)
			{
				currentCenter = bound.sphere.Center = bound.sphere.Center + velocityDirectionXZ;
				currentRadius = bound.sphere.Radius;
			}

			// 이동할 곳에서 충돌되는지 보기.
			std::vector<std::shared_ptr<Collider>> otherColliders;
			if (ColliderManager::main->CollisionChecksDirect(type, bound, otherColliders))
			{
				for (auto& otherCollider : otherColliders)
				{
					if (otherCollider->GetGroupID() != playerGroupID)
					{
						Vector3 otherColliderCenter = otherCollider->GetBoundCenter();
						Vector3 rayDir = otherColliderCenter - currentCenter;
						float rayDis = rayDir.Length();
						rayDir.Normalize();

						RayHit hitOtherCollider;
						if (otherCollider->RayCast({ currentCenter, rayDir }, rayDis, hitOtherCollider))
							if (hitOtherCollider)
								velocityDirectionXZ += hitOtherCollider.normal *
								std::max(velocityDirectionXZ.Length(), static_cast<float>(4.0f * Time::main->GetDeltaTime()));
					}
				}
			}
		}
	}
	float velocityDirectionY = velocity.y * Time::main->GetDeltaTime();
	nextPos += Vector3(velocityDirectionXZ.x,  velocityDirectionY, velocityDirectionXZ.z); // velocityDirectionXZ.y + velocityDirectionY



	float gitMargin = 0.1f;//오차 범위
	float upDistance = 1.0f;
	Vector3 upRayOffset = nextPos + Vector3::Up * upDistance;

	RayHit foundGround;
	std::vector<RayHit> hitList;
	if (auto isHit = ColliderManager::main->RayCastAll({ upRayOffset, Vector3::Down }, upDistance + gitMargin * 2, hitList))
	{
		for (auto& hit : hitList)
		{
			if (hit.gameObject->GetRoot() != GetOwner()->GetRoot())
			{
				if (upDistance + gitMargin >= hit.distance) {
					foundGround = hit;
				}
				break;
			}
		}
	}
	if (foundGround)
	{
		isGround = true;
		velocity.y = 0;
		nextPos.y = upRayOffset.y - foundGround.distance; //upRayOffset.y -
	}
	else
	{
		isGround = false;
		velocity.y -= 0.981;
	}

	// ------------- 공통 로직 ------------- 

	GetOwner()->_transform->SetWorldPosition(nextPos);
	velocity = velocity * (1 - (controlInfo.drag * Time::main->GetDeltaTime() * 60));
}


void PlayerController::Update2()
{
	Component::Update2();

	camera.lock()->Calculate();

	/*
	if (Input::main->GetMouse(KeyCode::LeftMouse))
	{
		auto mousePos = Input::main->GetMousePosition();
		auto camera = CameraManager::main->GetActiveCamera();
		auto cameraPos = camera->GetCameraPos();
		vec3 cameraDir = camera->GetScreenToWorldPosition(mousePos) - camera->GetCameraPos();
		cameraDir.Normalize();

		auto hit = ColliderManager::main->RayCast({ cameraPos, cameraDir }, 500);
		if (hit)
		{
			auto box = BoundingOrientedBox(hit.worldPos, Vector3::One, Quaternion::Identity);
			Gizmo::Width(0.02f);
			Gizmo::Box(box, ColliderManager::main->CollisionCheckDirect(CollisionType::Box, { .box = box }) ? Vector4(1, 0.5, 0, 1) : Vector4(0, 1, 0, 1));
			Gizmo::WidthRollBack();
		}
	}
	*/
}

void PlayerController::Enable()
{
	Component::Enable();
}

void PlayerController::Disable()
{
	Component::Disable();
}

void PlayerController::RenderBegin()
{
	Component::RenderBegin();
}

void PlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void PlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void PlayerController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void PlayerController::SetDestroy()
{
	Component::SetDestroy();
}

void PlayerController::Destroy()
{
	Component::Destroy();
}
