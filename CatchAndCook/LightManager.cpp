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
	ForwardLightParams params;
	// 여기 작업해야함.
	buffer->AddData(params);
}

ForwardLightSetter::~ForwardLightSetter()
{

}
