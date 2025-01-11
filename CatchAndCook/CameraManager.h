#pragma once

class Camera;
enum class CameraType;

class CameraManager
{
public:

	static unique_ptr<CameraManager> main;

	shared_ptr<Camera> GetCamera(CameraType type);

	void AddCamera(CameraType type, shared_ptr<Camera>& camera) { _cameras[type] = camera; }

	void SetActiveCamera(CameraType type);

	shared_ptr<Camera> GetActiveCamera() { return _activeCamera; }
	CameraType GetCameraType();

private:
	unordered_map<CameraType, shared_ptr<Camera>> _cameras;
	shared_ptr<Camera> _activeCamera{};

};

