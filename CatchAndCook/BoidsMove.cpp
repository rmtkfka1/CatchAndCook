#include "pch.h"
#include "BoidsMove.h"
#include "Transform.h"
#include <random>
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
    random_device urd;
    mt19937 gen(urd());
    uniform_real_distribution<float> dis(-20.0f, 20.0f);

    _objects.emplace_back(GetOwner().get());
    GetOwner()->_transform->SetVelocity({ dis(urd), dis(urd), dis(urd) });
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
    GameObject* object = this->GetOwner().get();
    vec3 pos = GetOwner()->_transform->GetLocalPosition();
    vec3 vel = GetOwner()->_transform->GetVelocity();

    vec3 separation(0.0f, 0.0f, 0.0f);
    vec3 alignment(0.0f, 0.0f, 0.0f);
    vec3 cohesion(0.0f, 0.0f, 0.0f);
    int neighborCount = 0;

    for (int32 j = 0; j < _objects.size(); ++j)
    {
        GameObject* other = _objects[j];

        if (other == object) continue;

        vec3 otherPos = other->_transform->GetLocalPosition();
        vec3 diff = pos - otherPos;
        float distance = diff.Length();

        if (distance > 0.0f && distance < _neighborDist)
        {
            if (distance < _desiredSeparation)
            {
                vec3 diffNormalized = diff / distance; // Normalize 없이 직접 계산
                separation += diffNormalized * (1.0f / distance);
            }

            alignment += other->_transform->GetVelocity();
            cohesion += otherPos;
            ++neighborCount;
        }
    }

    if (neighborCount > 0)
    {
        alignment /= static_cast<float>(neighborCount);
        cohesion /= static_cast<float>(neighborCount);
        cohesion = cohesion - pos;
    }
    else
    {
        alignment = vec3(0.0f, 0.0f, 0.0f);
        cohesion = vec3(0.0f, 0.0f, 0.0f);
    }

    float separationWeight = 32.0f;
    float alignmentWeight = 10.0f;
    float cohesionWeight = 32.0f;

    vec3 acceleration(0.0f, 0.0f, 0.0f);
    acceleration += separation * separationWeight;
    acceleration += alignment * alignmentWeight;
    acceleration += cohesion * cohesionWeight;

    /*if (acceleration.Length() > _maxForce) {
        acceleration = (acceleration / acceleration.Length()) * _maxForce;
    }*/

    float deltaTime = Time::main->GetDeltaTimeNow();
    vel += acceleration * deltaTime;

    float speed = vel.Length();
    if (speed > _maxSpeed) {
        vel = (vel / speed) * _maxSpeed;
    }

    pos += vel * deltaTime;

    this->GetOwner()->_transform->SetLocalPosition(pos);
    this->GetOwner()->_transform->SetVelocity(vel);
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
