#include "pch.h"
#include "LightComponent.h"

#include "LightManager.h"

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

	light.direction = vec3(-1.0f, -1.0f, 1.0f);
	light.position = vec3(0, 1000.0f, 0);
	light.direction.Normalize();

	light.material.ambient = vec3(0.2f, 0.2f, 0.2f);
	light.material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light.material.specular = vec3(1.0f, 1.0f, 1.0f);
	light.material.shininess = 61.0f;
	light.material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	light.strength = vec3(1.0f, 1.0f, 1.0f);
}

void LightComponent::Start()
{
	Component::Start();
}

void LightComponent::Update()
{
	Component::Update();
}

void LightComponent::Update2()
{
	Component::Update2();
}

void LightComponent::Enable()
{
	Component::Enable();
}

void LightComponent::Disable()
{
	Component::Disable();
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
}
