#include "pch.h"
#include "CameraManager.h"
#include "Camera.h"
unique_ptr<CameraManager> CameraManager::main = nullptr;

shared_ptr<Camera> CameraManager::GetCamera(CameraType type)
{
	return _cameras[type];
}

void CameraManager::Setting(CameraType type)
{
	_activeCamera = _cameras[type];
	_activeCamera->Update();
	_activeCamera->PushData();
	_activeCamera->SetData();
}

void CameraManager::SetActiveCamera(CameraType type)
{
	_activeCamera = _cameras[type];
}

CameraType CameraManager::GetCameraType()
{
	return _activeCamera->GetCameraType();
}
