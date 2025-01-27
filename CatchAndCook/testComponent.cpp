#include "pch.h"
#include "testComponent.h"
#include "GameObject.h"




testComponent::~testComponent()
{

}

void testComponent::Init()
{
	v.resize(24);

	for (int i = 0; i < v.size(); ++i)
	{
		float r=0;
		float g=0;
		float b=0;

		if (i % 3 == 0) 
		{
			r = 1;
		}
		else if (i % 3 == 1) 
		{
			g = 1;
		}
		else if (i % 3 == 2) 
		{
			b = 1;
		}

		v[i].testcolor = vec4(r, g, b, 0);
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
	material->SetHandle("Structured", _structuredBuffer._srvHandle);
}
