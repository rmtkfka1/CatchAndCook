#include "pch.h"
#include "testComponent.h"
#include "GameObject.h"




testComponent::~testComponent()
{

}

void testComponent::Init()
{
	v.resize(255);

	for (int i = 0; i < v.size(); ++i)
	{
		v[i].testcolor = vec4(1.0f, 0, 0, 0);
	}

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
	

}
