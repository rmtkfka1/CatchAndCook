#pragma once
#include "Scene.h"

class SceneManager
{
	std::unordered_map<std::string, std::shared_ptr<Scene>> _sceneTable;
	std::shared_ptr<Scene> _currentScene = nullptr;

public:
	static std::unique_ptr<SceneManager> main;
	std::shared_ptr<Scene> GetCurrentScene() { return _currentScene;};
	void AddScene(shared_ptr<Scene> scene);

	void ChangeScene(const shared_ptr<Scene>& nextScene);
	std::shared_ptr<Scene> FindScene(const std::string& name);

	friend class Scene;
};
