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

		if (collider->GetType() == CollisionType::Box)
		{
			if (other->GetType() == CollisionType::Box)
			{
				static int i = 0;
				cout << "콜백" << i++ << endl;

				DirectX::BoundingOrientedBox& myBox = collider->GetBoundUnion().box;
				DirectX::BoundingOrientedBox& otherBox = other->GetBoundUnion().box;

				XMVECTOR myCenter = XMLoadFloat3(&myBox.Center);
				XMVECTOR otherCenter = XMLoadFloat3(&otherBox.Center);
				XMVECTOR delta = XMVectorSubtract(myCenter, otherCenter);

				// AABB 크기 계산 (대충 OBB 반치수들로 계산)
				XMVECTOR myExtents = XMLoadFloat3(&myBox.Extents);
				XMVECTOR otherExtents = XMLoadFloat3(&otherBox.Extents);

				// 충돌한 만큼 계산 (단순 축 기준)
				XMFLOAT3 overlap;
				XMStoreFloat3(&overlap, XMVectorAbs(delta) - (myExtents + otherExtents));

				// MTV 방향 계산 (가장 적게 겹친 축 기준으로)
				XMFLOAT3 mtv = { 0, 0, 0 };
				if (overlap.x < overlap.y && overlap.x < overlap.z) {
					mtv.x = (delta.m128_f32[0] < 0 ? -1 : 1) * overlap.x;
				}
				else if (overlap.y < overlap.z) {
					mtv.y = (delta.m128_f32[1] < 0 ? -1 : 1) * overlap.y;
				}
				else {
					mtv.z = (delta.m128_f32[2] < 0 ? -1 : 1) * overlap.z;
				}

				// 현재 위치에서 MTV만큼 이동
				XMVECTOR newPos = XMLoadFloat3(&transform->GetLocalPosition()) + XMLoadFloat3(&mtv);
				transform->SetLocalPosition(newPos);
				

			}
		}
	}
}

void WallSlideController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

