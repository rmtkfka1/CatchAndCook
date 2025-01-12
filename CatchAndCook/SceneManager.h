#pragma once
#include "Scene.h"

enum class SceneType
{
	TestScene
};


class SceneManager
{

public:
	static std::unique_ptr<SceneManager> main;
	std::shared_ptr<Scene> GetCurrentScene() { return _currentScene;};
	shared_ptr<Scene> AddScene(SceneType type);

	void ChangeScene(const shared_ptr<Scene>& nextScene);
	std::shared_ptr<Scene> FindScene(SceneType type);

	friend class Scene;
private:
	std::unordered_map<SceneType, std::shared_ptr<Scene>> _sceneTable;
	std::shared_ptr<Scene> _currentScene = nullptr;
};
