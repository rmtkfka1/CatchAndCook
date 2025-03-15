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
    uniform_real_distribution<float> dis(-5.0f, 5.0f);
    uniform_real_distribution<float> dis2(-1.0f, 1.0f);
    _objects.emplace_back(GetOwner().get());

	float speed = dis(urd);
    GetOwner()->_transform->SetVelocity({ speed, speed,speed});
	GetOwner()->_transform->SetForward({ dis2(urd), dis2(urd), dis2(urd) });
}

void BoidsMove::Start()
{
}

void BoidsMove::Update()
{
    FindNeighbor();

    vec3 targetDir =/*Separate()*/  Cohesion();

	targetDir = vec3::Lerp(GetOwner()->_transform->GetForward(), targetDir, Time::main->GetDeltaTime());

    vec3 CurrnetPos = GetOwner()->_transform->GetLocalPosition();

	GetOwner()->_transform->SetForward(targetDir);
	GetOwner()->_transform->SetLocalPosition(CurrnetPos + targetDir  * Time::main->GetDeltaTime());

    //UpdateBoids();
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

	this->GetOwner()->_transform->SetForward(vel);
    this->GetOwner()->_transform->SetLocalPosition(pos);
    this->GetOwner()->_transform->SetVelocity(vel);
}



void BoidsMove::FindNeighbor()
{
    _neighbors.clear();

    for (int i = 0; i < _objects.size(); ++i)
    {

		if (_objects[i] == GetOwner().get())
		{
			continue;
		}
		
		float distance = (_objects[i]->_transform->GetLocalPosition() - GetOwner()->_transform->GetLocalPosition()).Length();

		if (distance > 0.0f && distance < _neighborDist)
		{
			_neighbors.emplace_back(_objects[i]);
		}
    }

}

vec3 BoidsMove::Separate()
{
    vec3 dir = vec3::Zero;

	size_t size = _neighbors.size();

	if (size == 0)
	{
		return dir;
	}

    for (int i = 0; i < size; ++i)
    {
        vec3 diff = GetOwner()->_transform->GetLocalPosition() - _neighbors[i]->_transform->GetLocalPosition();
        float distance = diff.Length();

        if (distance > 0.0f && distance< _desiredSeparation)
        {
            dir += diff / distance;  // 거리가 가까울수록 더 강한 분리 효과
        }
    }

    dir.Normalize();
	return dir;

}

vec3 BoidsMove::Align()
{

	size_t size = _neighbors.size();

	vec3 alignmentVec = GetOwner()->_transform->GetForward();

    for (int i = 0; i < size; ++i)
    {
        alignmentVec += _neighbors[i]->_transform->GetForward();
    }

	alignmentVec /= static_cast<float>(size);

	return alignmentVec;
}

vec3 BoidsMove::Cohesion()
{
 
    vec3 cohesionVec = vec3::Zero;

    if (_neighbors.size() > 0)
    {
        for (int i = 0; i < _neighbors.size(); i++)
        {
            cohesionVec += _neighbors[i]->_transform->GetLocalPosition();
        }
    }
    else
    {

        return cohesionVec;
    }

    cohesionVec /= _neighbors.size();
    cohesionVec -= GetOwner()->_transform->GetLocalPosition();
    cohesionVec.Normalize();
    return cohesionVec;
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
