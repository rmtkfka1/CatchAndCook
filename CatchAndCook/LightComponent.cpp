#include "pch.h"
#include "LightComponent.h"

#include "LightManager.h"
#include "Transform.h"

LightComponent::~LightComponent()
{
}

bool LightComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void LightComponent::Init()
{
	Component::Init();

	light = std::make_shared<Light>();

	light->material.ambient = vec3(0.2f, 0.2f, 0.2f);
	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light->material.specular = vec3(1.0f, 1.0f, 1.0f);
	light->material.shininess = 61.0f;
	//light->material.lightType = static_cast<int32>(LIGHT_TYPE::POINT_LIGHT);
	//light->strength = vec3(1.0f, 1.0f, 1.0f);
}

void LightComponent::Start()
{
	Component::Start();

	light->material.lightType = static_cast<int32>(type);
	light->intensity = intensity;
	light->strength = color;
	light->fallOffStart = 0;
	light->fallOffEnd = range;
	light->spotAngle = spotAngle * D2R;
	light->innerSpotAngle = innerSpotAngle * D2R;

	LightManager::main->PushLight(light);
}

void LightComponent::Update()
{
	Component::Update();
	light->direction = GetOwner()->_transform->GetForward();
	light->position = GetOwner()->_transform->GetWorldPosition();
}

void LightComponent::Update2()
{
	Component::Update2();
}

void LightComponent::Enable()
{
	Component::Enable();
	light->onOff = 1;
}

void LightComponent::Disable()
{
	Component::Disable();
	light->onOff = 0;
}

void LightComponent::RenderBegin()
{
	Component::RenderBegin();


}

void LightComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void LightComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void LightComponent::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void LightComponent::SetDestroy()
{
	Component::SetDestroy();
}

void LightComponent::Destroy()
{
	Component::Destroy();
	LightManager::main->RemoveLight(light);
}
