#include "pch.h"
#include "LightManager.h"
#include "Core.h"
#include "BufferPool.h"
#include "CameraManager.h"
#include "Transform.h"
#include "Camera.h"


unique_ptr<LightManager> LightManager::main ;

void LightManager::PushLight(Light light)
{
	_lightParmas.light[_lightParmas.lightCount] = light;
	_lightParmas.lightCount++;
}

void LightManager::SetData()
{
	Update();
	CBufferContainer* container = Core::main->GetBufferManager()->GetBufferPool(BufferType::LightParam)->Alloc(1);
	memcpy(container->ptr,(void*)&_lightParmas,sizeof(LightParams));
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(3,container->GPUAdress);
}

void LightManager::Update()
{
	_lightParmas.eyeWorldPos = CameraManager::main->GetActiveCamera()->GetCameraPos();

	//for(int i = 0; i < _lightParmas.lightCount; ++i)
	//{

	//	if(_lightParmas.light[i].material.lightType == static_cast<int32>(LIGHT_TYPE::SPOT_LIGHT))
	//	{
	//		_lightParmas.light[i].direction = _player->GetTransform()->GetLook();
	//		_lightParmas.light[i].position = _player->GetTransform()->GetLocalPosition();

	//	}

	//}


}
