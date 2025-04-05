#include "pch.h"
#include "PlantComponent.h"

PlantComponent::PlantComponent()
{
}

PlantComponent::~PlantComponent()
{
}


void PlantComponent::Init()
{
	/*_plantInfo.amplitude = 0.5f;
	_plantInfo.frequency = 0.8f;*/
}

void PlantComponent::Start()
{
	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->AddCbufferSetter(static_pointer_cast<PlantComponent>(shared_from_this()));
	}
}

void PlantComponent::Update()
{
}

void PlantComponent::Update2()
{
}

void PlantComponent::RenderBegin()
{



}

void PlantComponent::Enable()
{
}

void PlantComponent::Disable()
{
}


void PlantComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PlantComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

bool PlantComponent::IsExecuteAble()
{
	return false;
}

void PlantComponent::SetDestroy()
{

}

void PlantComponent::SetData(Material* material)
{
	//auto& cmdList =  Core::main->GetCmdList();
	//auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::PlantInfo)->Alloc(1);
	//memcpy(CbufferContainer->ptr, (void*)&_plantInfo, sizeof(PlantInfo));
	//cmdList->SetGraphicsRootConstantBufferView(8, CbufferContainer->GPUAdress);
}
