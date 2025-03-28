#include "pch.h"
#include "PlayerController.h"

#include "Camera.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "SkinnedMeshRenderer.h"
#include "Terrain.h"
#include "TerrainManager.h"
#include "Transform.h"

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

	SetMoveType(MoveType::Water);
}

void PlayerController::Update()
{
	if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
	{
		return;
	}

	Component::Update();

	if (moveType == MoveType::Water)
		for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
			renderer->GetOwner()->SetActiveSelf(false);
	if (moveType == MoveType::Field)
		for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
			renderer->GetOwner()->SetActiveSelf(true);

	CameraControl();
	MoveControl();


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
			//Gizmo::Box({ hit.worldPos, Vector3::One });
			//std::cout << hit.worldPos.x <<", "<< hit.worldPos.y <<","<< hit.worldPos.z << "\n";
			Gizmo::Width(0.5f);
			Gizmo::Ray(hit.worldPos, hit.normal, 2);
			Gizmo::WidthRollBack();
		}
	}
	//GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Forward * 10.0f * Time::main->GetDeltaTime());
}

void PlayerController::SetMoveType(MoveType moveType)
{
	this->moveType = moveType;

	switch (moveType)
	{
		case MoveType::Field:
		{
			controlInfo = fieldInfo;
			break;
		}
		case MoveType::Water:
		{
			controlInfo = waterInfo;
			break;
		}
	}
}


void PlayerController::CameraControl()
{
	auto cameraTransform = camera.lock()->GetOwner()->_transform;
	auto currentMousePosition = Input::main->GetMousePosition();
	_targetOffset = GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 1.1f;
	_currentOffset = Vector3::Lerp(_currentOffset, _targetOffset,
		(float)Time::main->GetDeltaTime() * controlInfo.cameraMoveSmooth
		* std::min(Vector3::Distance(_currentOffset, _targetOffset), 1.0f));
	Vector2 mouseDelta = currentMousePosition - Input::main->GetMousePrevPosition();
	// 카메라 방향 계산식

	Vector3 deltaEuler = Vector3(mouseDelta.y, mouseDelta.x, 0) * 0.01f * 0.5f;
	Vector3 calculateEuler = deltaEuler + _targetEuler;

	calculateEuler.z = 0.0f;
	if (calculateEuler.x * R2D <= -80) calculateEuler.x = -80 * D2R;
	if (calculateEuler.x * R2D >= 85) calculateEuler.x = 85 * D2R;

	_targetEuler = calculateEuler;
	_currentEuler = Vector3::Lerp(_currentEuler, _targetEuler, Time::main->GetDeltaTime() * controlInfo.cameraRotationSmooth);


	Vector3 cameraNextPosition = _currentOffset;
	Quaternion finalRotation = Quaternion::CreateFromYawPitchRoll(_currentEuler);
	Vector3 _currentDirection = Vector3::Transform(Vector3::Forward, finalRotation);


	switch (moveType)
	{
		case MoveType::Field:
		{
			finalRotation = Quaternion::CreateFromYawPitchRoll(_currentEuler);
			_currentDirection = Vector3::Transform(Vector3::Forward, finalRotation);

			float distance = 3.6;
			if (auto hit = ColliderManager::main->RayCast({ _currentOffset, -_currentDirection }, distance * 2))
				if (hit.gameObject->GetRoot() != GetOwner()->GetRoot())
					distance = std::min(hit.distance, distance) - 0.05f;


			cameraNextPosition = _currentOffset - _currentDirection * distance;
			break;
		}

		case MoveType::Water:
		{
			finalRotation = Quaternion::CreateFromYawPitchRoll(_targetEuler);
			_currentDirection = Vector3::Transform(Vector3::Forward, finalRotation);
			cameraNextPosition = _targetOffset;
			break;
		}
	}

	cameraTransform->SetWorldPosition(cameraNextPosition);
	cameraTransform->LookUp(_currentDirection, Vector3::Up);
}

void PlayerController::MoveControl()
{
	Quaternion lookRotation = Quaternion::Identity;
	switch (moveType)
	{
		case MoveType::Field:
		{
			lookRotation = Quaternion::CreateFromYawPitchRoll(Vector3(0, _targetEuler.y, 0));
			break;
		}

		case MoveType::Water:
		{
			lookRotation = Quaternion::CreateFromYawPitchRoll(Vector3(_targetEuler.x, _targetEuler.y, 0));
			break;
		}
	}

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


		velocity += targetLookWorldDirection * controlInfo.moveForce * Time::main->GetDeltaTime() * 60;
		if (velocity.Length() > controlInfo.maxSpeed) {
			velocity.Normalize();
			velocity *= controlInfo.maxSpeed;
		}
		GetOwner()->_transform->SetWorldRotation(currentLookWorldRotation);
	}


	Vector3 currentPos = GetOwner()->_transform->GetWorldPosition();
	Vector3 nextPos = currentPos + velocity * Time::main->GetDeltaTime();

	switch (moveType)
	{
		case MoveType::Field:
		{
			Vector3 upRayOffset = nextPos + Vector3::Up * 1.0f;

			if (auto hit = ColliderManager::main->RayCast({ upRayOffset, Vector3::Down }, 30))
				nextPos.y = upRayOffset.y - hit.distance;
			break;
		}
		case MoveType::Water:
		{
			break;
		}
	}

	// ------------- 공통 로직 ------------- 

	GetOwner()->_transform->SetWorldPosition(nextPos);
	velocity = velocity * (1 - (controlInfo.drag * Time::main->GetDeltaTime() * 60));
}


void PlayerController::Update2()
{
	Component::Update2();
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
