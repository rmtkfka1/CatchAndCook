#include "pch.h"
#include "SeaPlayerController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Transform.h"
#include "TerrainManager.h"
#include "Terrain.h"
#include "Gizmo.h"
SeaPlayerController::SeaPlayerController()
{
}

SeaPlayerController::~SeaPlayerController()
{
}
void SeaPlayerController::Init()
{
	
}

void SeaPlayerController::Start()
{
	_camera = CameraManager::main->GetCamera(CameraType::SeaCamera);
	_transform = GetOwner()->_transform;
	_collider = GetOwner()->GetComponent<Collider>();

	if (auto terrian = SceneManager::main->GetCurrentScene()->Find(L"Terrain"))
	{
		cout << "찾음" << endl;
		_terrian = terrian->GetComponent<Terrain>();
	}


	GetOwner()->_transform->SetForward(vec3(0, 0, 1));
	GetOwner()->_transform->SetRight(vec3(1, 0, 0));
	GetOwner()->_transform->SetUp(vec3(0, 1, 0));

}

void SeaPlayerController::Update()
{
    float dt = Time::main->GetDeltaTime();
    Quaternion rotation = CalCulateYawPitchRoll();

	_transform->SetWorldRotation(rotation);
    _camera->SetCameraRotation(rotation);

    vec3 inputDir = vec3::Zero;

    // --- 입력 처리 ---
    if (Input::main->GetKey(KeyCode::UpArrow)) inputDir += vec3::Forward;
    if (Input::main->GetKey(KeyCode::DownArrow)) inputDir += vec3::Backward;
    if (Input::main->GetKey(KeyCode::LeftArrow)) inputDir += vec3::Left;
    if (Input::main->GetKey(KeyCode::RightArrow)) inputDir += vec3::Right;
    if (Input::main->GetKey(KeyCode::Space)) inputDir += vec3(0, 2, 0);

    if (inputDir != vec3::Zero)
    {
        inputDir.Normalize();
        vec3 moveDir = vec3::Transform(inputDir, rotation);
        _velocity += moveDir * _moveForce * dt;

        if (_velocity.Length() > _maxSpeed)
        {
            _velocity.Normalize();
            _velocity *= _maxSpeed;
        }
    }

    vec3 currentPos = _transform->GetWorldPosition();
    vec3 nextPos = currentPos + _velocity * dt;
    vec3 headOffset = vec3(0, _cameraHeightOffset, 0);
    vec3 rotatedHeadOffset = vec3::Transform(headOffset, rotation);
    vec3 nextHeadPos = nextPos + rotatedHeadOffset + _transform->GetForward()*0.2f;



	vec3 dir = _velocity;
    dir.Normalize();
    float maxDist = _playerRadius;
	auto ray = ColliderManager::main->RayCastForMyCell({ nextHeadPos, dir }, maxDist, GetOwner());

    if (ray.isHit)
    {
        vec3 normal = ray.normal;

        _velocity = _velocity - _velocity.Dot(normal) * normal;

        float penetrationBuffer = 0.01f;
        nextPos += normal * penetrationBuffer;
        nextHeadPos += normal * penetrationBuffer;
    }
 


    // 지형 충돌 처리
    float terrainHeight = _terrian->GetLocalHeight(nextHeadPos);
    if (nextHeadPos.y < terrainHeight + 2.5f)
    {
        float deltaY = (terrainHeight + 2.5f) - nextHeadPos.y;
        nextPos.y += deltaY;
        nextHeadPos.y += deltaY;
    }



    // 최종 위치 적용
    _transform->SetWorldPosition(nextPos);
    _camera->SetCameraPos(nextHeadPos);
    _velocity *= (1 - (_resistance * dt));
}

void SeaPlayerController::KeyUpdate(vec3& inputDir, Quaternion& rotation, float dt)
{
    if (Input::main->GetKey(KeyCode::UpArrow))
    {
        inputDir += vec3::Forward;
    }
    if (Input::main->GetKey(KeyCode::DownArrow))
    {
        inputDir += vec3::Backward;
    }
    if (Input::main->GetKey(KeyCode::LeftArrow))
    {
        inputDir += vec3::Left;
    }
    if (Input::main->GetKey(KeyCode::RightArrow))
    {
        inputDir += vec3::Right;
    }
    if (Input::main->GetKey(KeyCode::Space))
    {
        inputDir += vec3(0, 2, 0);
    }

    if (inputDir != vec3::Zero)
    {
        inputDir.Normalize();
        vec3 moveDir = vec3::Transform(inputDir, rotation);
        _velocity += moveDir * _moveForce * dt;

        if (_velocity.Length() > _maxSpeed)
        {
            _velocity.Normalize();
            _velocity = _velocity * _maxSpeed;
        }
    }

}

void SeaPlayerController::Update2()
{
}

void SeaPlayerController::Enable()
{
}

void SeaPlayerController::Disable()
{
}

void SeaPlayerController::RenderBegin()
{
}

void SeaPlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SeaPlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SeaPlayerController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void SeaPlayerController::SetDestroy()
{
}

void SeaPlayerController::Destroy()
{
}

Quaternion SeaPlayerController::CalCulateYawPitchRoll()
{
	if (Input::main->IsMouseLock() == false)
	{
		static vec2 lastMousePos = Input::main->GetMousePosition();

		if (Input::main->GetKeyDown(KeyCode::BackQoute))
		{
			lastMousePos = Input::main->GetMousePosition();
		}
	
		vec2 currentMousePos = Input::main->GetMousePosition();

		vec2 delta = (currentMousePos - lastMousePos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
		_pitch = std::clamp(_pitch, -89.0f, 89.0f);
		_roll = 0;

		lastMousePos = currentMousePos;
	}

	else
	{
		vec2 currentMousePos = Input::main->GetMousePosition();
		vec2 centerPos = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		vec2 delta = (currentMousePos - centerPos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
		_pitch = std::clamp(_pitch, -89.0f, 89.0f);
		_roll = 0;

		POINT center;
		center.x = static_cast<LONG>(WINDOW_WIDTH/2);
		center.y = static_cast<LONG>(WINDOW_HEIGHT/2);
		ClientToScreen(Core::main->GetHandle(), &center);
		SetCursorPos(center.x, center.y);
	
	}


    return  Quaternion::CreateFromYawPitchRoll(_yaw * D2R, _pitch * D2R, 0);
};