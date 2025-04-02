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

    vec3 inputDir = vec3::Zero;
    if (Input::main->GetKey(KeyCode::UpArrow))
        inputDir += vec3::Forward;

    if (Input::main->GetKey(KeyCode::DownArrow))
        inputDir += vec3::Backward;

    if (Input::main->GetKey(KeyCode::LeftArrow))
        inputDir += vec3::Left;

    if (Input::main->GetKey(KeyCode::RightArrow))
        inputDir += vec3::Right;

    if (Input::main->GetKey(KeyCode::Space))
        inputDir += vec3(0, 2, 0); // 점프 또는 위로 이동

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

    _transform->SetLocalRotation(rotation);
    _camera->SetCameraRotation(rotation);

    vec3 currentPlayerPos = _transform->GetWorldPosition();
    vec3 nextPlayerPos = currentPlayerPos + _velocity * dt;

    vec3 headOffset = vec3(0, _cameraHeightOffset, 0);
    vec3 rotatedHeadOffset = vec3::Transform(headOffset, rotation);
    vec3 nextCameraPos = nextPlayerPos + rotatedHeadOffset + _transform->GetForward() * 0.1f;

    vec3 dir = _velocity;
    dir.Normalize();

    Ray lookRay;
    lookRay.position = nextCameraPos;
    lookRay.direction = dir;
    float maxDist = 3.0f;

    auto hit = ColliderManager::main->RayCast(lookRay, maxDist, GetOwner());

    if (hit.isHit)
    {
        // 슬라이딩 벡터 계산
        vec3 slidingVector = _velocity - hit.normal * _velocity.Dot(hit.normal);

        // 충돌 후 위치 수정 (떨림 방지)
        const float minDist = 0.01f; // 최소 거리
        if (hit.distance > minDist)
        {
            nextPlayerPos = hit.worldPos + slidingVector * dt;
            nextCameraPos = hit.worldPos + rotatedHeadOffset + slidingVector * dt;
        }
        else
        {
            // 속도가 너무 낮을 경우 정지
            _velocity = vec3::Zero;
        }

        _velocity *= 0.5f; // 충돌 시 속도 감소
    }

    float terrainHeight = _terrian->GetLocalHeight(nextCameraPos);
    if (nextCameraPos.y < terrainHeight + 3.0f)
    {
        float deltaY = (terrainHeight + 3.0f) - nextCameraPos.y;
        nextPlayerPos.y += deltaY;
        nextCameraPos.y += deltaY;
    }

    _transform->SetWorldPosition(nextPlayerPos);
    _camera->SetCameraPos(nextCameraPos);

    // 속도 보간 처리 (부드러운 감속)
    _velocity = vec3::Lerp(_velocity, vec3::Zero, dt * _resistance);

    CameraManager::main->Setting(CameraType::SeaCamera);

    // 디버그용 카메라 렌더링
    {
        Gizmo::Width(0.1f);
        auto o = _camera->GetCameraPos();
        auto f = _camera->GetCameraLook();
        auto u = _camera->GetCameraUp();
        auto r = _camera->GetCameraRight();
        Gizmo::Line(o, o + f, Vector4(0, 0, 1, 1));
        Gizmo::Line(o, o + u, Vector4(0, 1, 0, 1));
        Gizmo::Line(o, o + r, Vector4(1, 0, 0, 1));
    }
}

void SeaPlayerController::UpdatePlayerAndCamera(float dt, Quaternion& rotation)
{
    _transform->SetLocalRotation(rotation);
    _camera->SetCameraRotation(rotation);

    vec3 currentPlayerPos = _transform->GetWorldPosition();
    vec3 nextPlayerPos = currentPlayerPos + _velocity * dt;

    vec3 headOffset = vec3(0, _cameraHeightOffset, 0);
    vec3 rotatedHeadOffset = vec3::Transform(headOffset, rotation);
    vec3 nextCameraPos = nextPlayerPos + rotatedHeadOffset + _transform->GetForward() * 0.1f;

    vec3 dir = _velocity;
    dir.Normalize();

    Ray lookRay;
    lookRay.position = nextCameraPos;  
    lookRay.direction = dir; 
    float maxDist = 3.0f;

    auto hit = ColliderManager::main->RayCast(lookRay, maxDist, GetOwner());

    if (hit.isHit)
    {
        nextPlayerPos += hit.normal * hit.distance;
        nextCameraPos += hit.normal * hit.distance;

        _velocity *= 0.3f;


    }

 
    float terrainHeight = _terrian->GetLocalHeight(nextCameraPos);
    if (nextCameraPos.y < terrainHeight + 3.0f)
    {
        float deltaY = (terrainHeight + 3.0f) - nextCameraPos.y;
        nextPlayerPos.y += deltaY;
        nextCameraPos.y += deltaY;
    }

    _transform->SetWorldPosition(nextPlayerPos);
    _camera->SetCameraPos(nextCameraPos);

    _velocity *= (1 - (_resistance * dt));
    CameraManager::main->Setting(CameraType::SeaCamera);
};

void SeaPlayerController::KeyUpdate(vec3& inputDir, Quaternion& rotation, float dt)
{
    if (Input::main->GetKey(KeyCode::UpArrow))
        inputDir += vec3::Forward;

    if (Input::main->GetKey(KeyCode::DownArrow))
        inputDir += vec3::Backward;

    if (Input::main->GetKey(KeyCode::LeftArrow))
        inputDir += vec3::Left;

    if (Input::main->GetKey(KeyCode::RightArrow))
        inputDir += vec3::Right;

    if (Input::main->GetKey(KeyCode::Space))
        inputDir += vec3(0, 2, 0); // 점프 or 위로 이동

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