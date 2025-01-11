#include "pch.h"
#include "CameraManager.h"

unique_ptr<CameraManager> CameraManager::main = nullptr;

shared_ptr<Camera> CameraManager::GetCamera(CameraType type)
{
	return _cameras[type];
}

void CameraManager::SetActiveCamera(CameraType type)
{
	_activeCamera = _cameras[type];
}

CameraType CameraManager::GetCameraType()
{
	return CameraType();
}
