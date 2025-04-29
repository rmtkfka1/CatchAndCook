#include "pch.h"
#include "LightManager.h"
#include "Core.h"
#include "BufferPool.h"
#include "CameraManager.h"
#include "Transform.h"
#include "Camera.h"
#include "ImguiManager.h"

unique_ptr<LightManager> LightManager::main ;

void LightManager::Init()
{

}

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
	_strBuffer = Core::main->GetBufferManager()->GetStructuredBufferPool(BufferType::LightDataParam);
	_strBuffer->Clear();
	Update();

	CBufferContainer* container = Core::main->GetBufferManager()->GetBufferPool(BufferType::LightHelperParam)->Alloc(1);
	memcpy(container->ptr, (void*)&_lightParmas, sizeof(LightHelperParams));

	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(3, container->GPUAdress);
	Core::main->GetCmdList()->SetComputeRootConstantBufferView(3, container->GPUAdress);

	tableContainer tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(tableContainer.CPUHandle, _strBuffer->GetSRVHandle(), 0);

	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_LIGHTPARM_TABLE_INDEX, tableContainer.GPUHandle);
	Core::main->GetCmdList()->SetComputeRootDescriptorTable(11, tableContainer.GPUHandle);
}

std::shared_ptr<Light> LightManager::GetMainLight()
{
	for (auto& light : _lights)
	{
		if (_mainLights == nullptr && light->material.lightType == 0)
		{
			_mainLights = light;
		}
	}
	return _mainLights;
}


void LightManager::Update()
{
	_lightParmas.eyeWorldPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
	_lightParmas.lightCount = _lights.size();

	/*_lightParmas.lightCount = 0;
	for (auto& light : _lights)
		if (light->onOff == 1 && light->material.lightType == 0)
		{
			_lightParmas.light[_lightParmas.lightCount++] = *light.get();
			break;
		}
	std::vector<std::shared_ptr<Light>> _lightSorts;
	_lightSorts.reserve(LightManager::main->_lights.size());
	_lightSorts.insert(_lightSorts.end(), LightManager::main->_lights.begin(), LightManager::main->_lights.end());
	std::ranges::sort(_lightSorts, [&](const std::shared_ptr<Light>& light1, const std::shared_ptr<Light>& light2) {
		return (light1->position - _lightParmas.eyeWorldPos).LengthSquared() < (light2->position - _lightParmas.eyeWorldPos).LengthSquared();
		});
	for (auto& light : _lightSorts)
	{
		if (light->onOff == 1 && light->material.lightType != 0)
		{
			if (_lightParmas.light.size() <= _lightParmas.lightCount)
				break;
			_lightParmas.light[_lightParmas.lightCount] = *light.get();
			_lightParmas.lightCount++;
		}
	}*/
	for (auto& ele : _lights)
	{
		if (ele) 
		{
			if(ele->onOff==1)
				_strBuffer->AddData(*ele); 
		}
	}
}

//
//void ForwardLightSetter::Init(GameObject* object)
//{
//	this->object = object;
//}
//
//void ForwardLightSetter::SetData(StructuredBuffer* buffer, Material* material)
//{
//	std::vector<std::shared_ptr<Light>> _lightForwards;
//	ForwardLightParams params;
//	Vector3 worldPos = this->object->_transform->GetWorldPosition();
//
//	_lightForwards.reserve(LightManager::main->_lights.size());
//	_lightForwards.insert(_lightForwards.end(), LightManager::main->_lights.begin(), LightManager::main->_lights.end());
//	std::ranges::sort(_lightForwards, [&](const std::shared_ptr<Light>& light1, const std::shared_ptr<Light>& light2) {
//		return (light1->position - worldPos).LengthSquared() < (light2->position - worldPos).LengthSquared();
//		});
//
//	for (int i = 0; i < std::min(5, static_cast<int>(_lightForwards.size())); i++)
//	{
//		auto& light = _lightForwards[i];
//		params.lights[params.lightCount] = *light.get();
//		params.lightCount++;
//	}
//
//	buffer->AddData(params);
//}
//
//
//
//ForwardLightSetter::~ForwardLightSetter()
//{
//
//}
