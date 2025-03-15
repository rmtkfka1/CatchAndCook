#include "pch.h"
#include "testComponent.h"
#include "GameObject.h"
#include <random>
#include "GameObject.h"
#include "Transform.h"
#include "CameraManager.h"
#include "Camera.h"
#include "ColliderManager.h"
std::random_device dre;
std::mt19937 gen(dre());
std::uniform_real_distribution<float> uid(0.0f, 1.0f);


testComponent::~testComponent()
{

}

void testComponent::Init()
{


}

void testComponent::Start()
{

}

void testComponent::Update()
{
	const float speed = 10.0f;
	const float dt =Time::main->GetDeltaTime() *speed;

	std::shared_ptr<Transform> transform =GetOwner()->_transform;

	if(transform==nullptr)
		return;

	if(Input::main->GetKey(KeyCode::UpArrow))
	{

		auto direction = transform->GetForward();
		auto& pos =transform->GetLocalPosition();
		transform->SetLocalPosition(pos + direction * dt);
	
	}

	if(Input::main->GetKey(KeyCode::DownArrow))
	{

		auto direction = transform->GetForward();
		auto& pos =transform->GetLocalPosition();
		transform->SetLocalPosition(pos - direction * dt);

	}

	if (Input::main->GetKey(KeyCode::RightArrow))
	{
		auto direction = transform->GetRight();
		auto& pos = transform->GetLocalPosition();
		RayHit hit = ColliderManager::main->RayCast(Ray(transform->GetWorldPosition() + transform->GetRight() * 5.01f, direction), 10000.0f);

		cout << hit.distance << endl;

		if (hit.distance>=0)
		{
			transform->SetLocalPosition(pos + direction * dt);
		}
		else
		{
			{
				vec3 slideVec = direction - hit.normal * direction.Dot(hit.normal);
				slideVec.Normalize();

				transform->SetLocalPosition(pos + slideVec * dt);

			}
		}
	}

	if (Input::main->GetKey(KeyCode::LeftArrow))
	{
		auto direction = transform->GetRight();
		auto& pos = transform->GetLocalPosition();
		transform->SetLocalPosition(pos - direction * dt);
	}


}

void testComponent::Update2()
{

}

void testComponent::Enable()
{

}

void testComponent::Disable()
{

}

void testComponent::Destroy()
{

}

void testComponent::RenderBegin()
{

}

void testComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	cout << "CollisionBegin\n"; 
}

void testComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);
}

bool testComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void testComponent::SetDestroy()
{
	IDelayDestroy::SetDestroy();
}

void testComponent::DestroyComponentOnly()
{

}

