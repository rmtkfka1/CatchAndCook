#include "pch.h"
#include "testComponent.h"
#include "GameObject.h"
#include <random>


std::random_device dre;
std::mt19937 gen(dre());
std::uniform_real_distribution<float> uid(0.0f, 1.0f);


testComponent::~testComponent()
{

}

void testComponent::Init()
{
	v.resize(24);

	_structuredBuffer.Init(v);

}

void testComponent::Start()
{
	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->AddSetter(static_pointer_cast<testComponent>(shared_from_this()));
	}
}

void testComponent::Update()
{

	static float time = 0;
	
	time += Time::main->GetDeltaTime();

	if (time > 1.0f)
	{
		for (int i = 0; i < v.size(); ++i)
		{
			float r = uid(gen);
			float g = uid(gen);
			float b = uid(gen);
			v[i].testcolor = vec4(r, g, b, 0); 
		}

		_structuredBuffer.Upload(v);
		time = 0;
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
	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->RemoveSetters(static_pointer_cast<testComponent>(shared_from_this()));
	}
}

void testComponent::RenderBegin()
{

}

void testComponent::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void testComponent::SetDestroy()
{
	IDelayDestroy::SetDestroy();
}

void testComponent::DestroyComponentOnly()
{

}

void testComponent::PushData()
{

}
 

void testComponent::SetData(Material* material)
{
	material->SetHandle("Structured", _structuredBuffer.GetSRVHandle());
}
