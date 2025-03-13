#include "pch.h"
#include "BoidsMove.h"
#include "Transform.h"

//Cohension 
//Alignment
//Separation

vector<GameObject*>BoidsMove:: _objects;


BoidsMove::BoidsMove()
{

}

BoidsMove::~BoidsMove()
{
}

void BoidsMove::SetDestroy()
{
}

void BoidsMove::Init()
{
    _objects.emplace_back(GetOwner().get());
    GetOwner()->_transform->SetVelocity({ 1.0f, 1.0f, 1.0f });
}

void BoidsMove::Start()
{
}

void BoidsMove::Update()
{
    UpdateBoids();
}

void BoidsMove::Update2()
{

}

void BoidsMove::Enable()
{
}

void BoidsMove::Disable()
{
}

void BoidsMove::Destroy()
{
}

void BoidsMove::RenderBegin()
{
}

void BoidsMove::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void BoidsMove::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

bool BoidsMove::IsExecuteAble()
{
	return false;
}

void BoidsMove::UpdateBoids()
{
    for (int32 i = 0; i < _objects.size(); ++i)
    {
        GameObject* object = _objects[i];
		vec3 pos = GetOwner()->_transform->GetLocalPosition();
		vec3 vel = GetOwner()->_transform->GetVelocity();

        vec3 separation(0.0f, 0.0f, 0.0f); // 분리 force
        vec3 alignment(0.0f, 0.0f, 0.0f);  // 정렬 force
        vec3 cohesion(0.0f, 0.0f, 0.0f);   // 응집 force
        int neighborCount = 0;

        for (int32 j = 0; j < _objects.size(); ++j)
        {
            if (i == j) continue;

            GameObject* other = _objects[j];
            vec3 otherPos = other->_transform->GetLocalPosition();

            vec3 diff = pos - otherPos;

            float distance = diff.Length();

            if (distance > 0.0f && distance < _neighborDist)
            {
                // 분리: 너무 가까우면 반대 방향으로 힘을 적용
                if (distance < _desiredSeparation)
                {
                    vec3 diffNormalized = diff;
                    diffNormalized.Normalize();
                    // 거리에 반비례하는 힘 (거리가 짧을수록 강한 힘)
                    separation += diffNormalized * (1.0f / distance);
                }

                // 정렬: 이웃의 평균 속도 계산
                alignment += other->_transform->GetVelocity();
                // 응집: 이웃의 평균 위치 계산 (후에 목표 방향으로 전환)
                cohesion += otherPos;
                ++neighborCount;
            }
        }

        // 이웃이 존재하면 평균을 내어 정렬 및 응집 계산
        if (neighborCount > 0)
        {
            alignment /= static_cast<float>(neighborCount);
            cohesion /= static_cast<float>(neighborCount);
            // 응집은 현재 위치에서 이웃의 중심으로 향하는 벡터
            cohesion = cohesion - pos;
        }

        float separationWeight = 0.5f;
        float alignmentWeight = 1.0f;
        float cohesionWeight = 1.0f;

        vec3 acceleration = vec3(0.0f, 0.0f, 0.0f);
        acceleration += separation * separationWeight;
        acceleration += alignment * alignmentWeight;
        acceleration += cohesion * cohesionWeight;

        acceleration = LimitForce(acceleration, _maxForce);

        vel += acceleration * Time::main->GetDeltaTimeNow();

        float speed = vel.Length();

        if (speed > _maxSpeed)
        {
            vel.Normalize();
            vel = vel * _maxSpeed;
        }

        pos += vel * Time::main->GetDeltaTimeNow();

		this->GetOwner()->_transform->SetLocalPosition(pos);
		this->GetOwner()->_transform->SetVelocity(vel);
    }
}

vec3 BoidsMove::LimitForce(vec3& force, float maxForce)
{
    force.Normalize();

    float length = force.Length();
    if (length > maxForce) {
        return force * maxForce;
    }
    return force;
};
