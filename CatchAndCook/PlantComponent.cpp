#include "pch.h"
#include "PlantComponent.h"

COMPONENT(PlantComponent)

PlantComponent::PlantComponent()
{
}

PlantComponent::~PlantComponent()
{
}


void PlantComponent::Init()
{
	
}

void PlantComponent::Start()
{
	if (auto renderer =GetOwner()->GetRenderer())
	{
		renderer->AddStructuredSetter(static_pointer_cast<PlantComponent>(shared_from_this()), BufferType::SeaPlantParam);
		_renderBase = renderer;
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

void PlantComponent::SetData(StructuredBuffer* buffer, Material* material)
{
	PlantInfo info;
	info.amplitude;
	info.color = material->GetPropertyVector("_Color");
	info.amplitude = material->GetPropertyFloat("_Amplitude");
	info.frequency = material->GetPropertyFloat("_frequency");

	BoundingBox& box = _renderBase.lock()->GetOriginBound();
	info.boundsSizeY =box.Extents.y*2;
	info.boundsCenterY = box.Center.y;



	buffer->AddData(info);
}



