#include "pch.h"
#include "LightManager.h"
#include "Core.h"
#include "BufferPool.h"
#include "CameraManager.h"
#include "Transform.h"
#include "Camera.h"
#include "ImguiManager.h"

unique_ptr<LightManager> LightManager::main ;

void LightManager::PushLight(const std::shared_ptr<Light>& light)
{
	if (std::ranges::find(_lights, light) == _lights.end())
		_lights.push_back(light);
}

void LightManager::RemoveLight(const std::shared_ptr<Light>& light)
{
	auto it = std::ranges::find(_lights, light);
	if (it != _lights.end())
		_lights.erase(it);
}

void LightManager::SetData()
{
	Update();

	CBufferContainer* container = Core::main->GetBufferManager()->GetBufferPool(BufferType::LightParam)->Alloc(1);
	memcpy(container->ptr,(void*)&_lightParmas,sizeof(LightParams));
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(3, container->GPUAdress);
	Core::main->GetCmdList()->SetComputeRootConstantBufferView(3, container->GPUAdress);
}

void LightManager::SetDataForward(void* addr, const std::shared_ptr<GameObject>& obj)
{

}

void LightManager::Update()
{
	_lightParmas.eyeWorldPos = CameraManager::main->GetActiveCamera()->GetCameraPos();

	_lightParmas.lightCount = 0;

	for (auto& light : _lights)
	{
		if (light->onOff == 1)
		{
			if (light->material.lightType == static_cast<int>(LIGHT_TYPE::SPOT_LIGHT))
			{
				light->direction = CameraManager::main->GetActiveCamera()->GetCameraLook();
				light->position = CameraManager::main->GetActiveCamera()->GetCameraPos();
			}

			_lightParmas.light[_lightParmas.lightCount] = *light.get();
			_lightParmas.lightCount++;
		}
	}

#ifdef IMGUI_ON
	for (int i = 0; i < _lightParmas.lightCount; ++i)
	{
		if (ImguiManager::main->_light)
			_lightParmas.light[i].position = ImguiManager::main->_light->_transform->GetWorldPosition();
	}
#endif 

}


void ForwardLightSetter::Init(GameObject* object)
{
	this->object = object;
}

void ForwardLightSetter::SetData(StructuredBuffer* buffer)
{
	std::vector<std::shared_ptr<Light>> _lightForwards;
	ForwardLightParams params;
	Vector3 worldPos = this->object->_transform->GetWorldPosition();

	_lightForwards.reserve(LightManager::main->_lights.size());
	_lightForwards.insert(_lightForwards.end(), LightManager::main->_lights.begin(), LightManager::main->_lights.end());
	std::ranges::sort(_lightForwards, [&](const std::shared_ptr<Light>& light1, const std::shared_ptr<Light>& light2) {
			return (light1->position - worldPos).LengthSquared() < (light2->position - worldPos).LengthSquared();
		});

	for(int i=0;i<std::min(5, static_cast<int>(_lightForwards.size()));i++)
	{
		auto& light = _lightForwards[i];
		params.lights[params.lightCount] = *light.get();
		params.lightCount++;
	}

	buffer->AddData(params);
}

ForwardLightSetter::~ForwardLightSetter()
{

}
