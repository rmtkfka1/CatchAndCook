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

	if (other->GetOwner()->HasTag(GameObjectTag::Wall))
	{
        auto& transform =  GetOwner()->_transform;

		if (other->GetType() == CollisionType::Box)
		{
            BoundingOrientedBox& box = other->GetBoundUnion().box;

			vec3 contactPoint = ComputeContactPoint(box, transform->GetForward());

            // ✅ 법선 벡터 계산
            vec3 normal = ComputeOBBContactNormal(box, contactPoint);

            // ✅ 법선 벡터 출력
            std::cout << "Contact Normal: (" << normal.x << ", " << normal.y << ", " << normal.z << ")" << std::endl;
		
		}

	}
}

void WallSlideController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

vec3 WallSlideController::ComputeContactPoint(const BoundingOrientedBox& box, const vec3& direction)
{
    // ✅ 1. OBB 중심
    vec3 center = box.Center;

    // ✅ 2. OBB의 회전 행렬 구하기 (Quaternion → Rotation Matrix 변환)
    Matrix rotationMatrix = Matrix::CreateFromQuaternion(box.Orientation);

    // ✅ 3. OBB의 로컬 축 벡터 (Right, Up, Forward) 구하기
    vec3 right = vec3(rotationMatrix._11, rotationMatrix._12, rotationMatrix._13); // X축
    vec3 up = vec3(rotationMatrix._21, rotationMatrix._22, rotationMatrix._23); // Y축
    vec3 forward = vec3(rotationMatrix._31, rotationMatrix._32, rotationMatrix._33); // Z축

    // ✅ 4. 충돌 방향과 OBB 크기(Extents)를 이용하여 충돌 지점 계산
    vec3 contactPoint = center
        + right * (direction.Dot(right) > 0 ? box.Extents.x : -box.Extents.x)
        + up * (direction.Dot(up) > 0 ? box.Extents.y : -box.Extents.y)
        + forward * (direction.Dot(forward) > 0 ? box.Extents.z : -box.Extents.z);

    return contactPoint;
}

vec3 WallSlideController::ComputeOBBContactNormal(const BoundingOrientedBox& box, const vec3& contactPoint)
{
    // OBB 중심과 충돌 지점 간의 차이 계산 (로컬 위치 변환)
    Vector3 localPos = contactPoint - box.Center;

    // ✅ 1. OBB의 `Orientation`(Quaternion)을 회전 행렬로 변환
    Matrix rotationMatrix = Matrix::CreateFromQuaternion(box.Orientation);

    // ✅ 2. 변환된 행렬에서 로컬 X, Y, Z 축을 추출
    Vector3 right = Vector3(rotationMatrix._11, rotationMatrix._12, rotationMatrix._13); // X축
    Vector3 up = Vector3(rotationMatrix._21, rotationMatrix._22, rotationMatrix._23); // Y축
    Vector3 forward = Vector3(rotationMatrix._31, rotationMatrix._32, rotationMatrix._33); // Z축

    // ✅ 3. 충돌 위치가 가장 가까운 축을 기준으로 법선 계산
    float xProj = fabs(localPos.Dot(right));
    float yProj = fabs(localPos.Dot(up));
    float zProj = fabs(localPos.Dot(forward));

    Vector3 normal;
    if (xProj > yProj && xProj > zProj)
        normal = right * (localPos.Dot(right) > 0 ? 1.0f : -1.0f);
    else if (yProj > xProj && yProj > zProj)
        normal = up * (localPos.Dot(up) > 0 ? 1.0f : -1.0f);
    else
        normal = forward * (localPos.Dot(forward) > 0 ? 1.0f : -1.0f);

    return normal; // 법선 벡터 반환
}

