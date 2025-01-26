#include "pch.h"
#include "testComponent.h"
#include "GameObject.h"




void testComponent::Init()
{
	_test.testcolor = vec4(0.4f, 0.3f, 0, 0);
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

}

void testComponent::RenderBegin()
{
	PushData();
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
	_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::TestParam)->Alloc(1);
	memcpy(_cbufferContainer->ptr, (void*)&_test, sizeof(test));
}

void testComponent::SetData(shared_ptr<Shader> shader)
{
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(shader->GetRegisterIndex("test"), _cbufferContainer->GPUAdress);
}
