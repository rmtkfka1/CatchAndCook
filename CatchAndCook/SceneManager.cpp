#include "pch.h"
#include "SceneManager.h"

#include "GameObject.h"

std::unique_ptr<SceneManager> SceneManager::main = nullptr;

void SceneManager::AddScene(shared_ptr<Scene> scene)
{
	_sceneTable.emplace(scene->_name, scene);

	if (_currentScene == nullptr)
		_currentScene = scene;
}


void SceneManager::ChangeScene(const shared_ptr<Scene>& nextScene)
{
	auto currentScene = GetCurrentScene();
	std::vector<std::shared_ptr<GameObject>> dontObj;
	if (currentScene != nullptr)
	{
		for (auto& obj : _currentScene->_dont_destroy_gameObjects)
			if (!obj->IsDestroy())
				obj->GetChildsAll(dontObj);
	}
	for (auto& obj : dontObj)
	{
		nextScene->AddGameObject(obj);
		nextScene->_dont_destroy_gameObjects.push_back(obj);
		currentScene->RemoveGameObject(obj);
	}

	currentScene->_dont_destroy_gameObjects.clear();
	_currentScene = nextScene;
}

std::shared_ptr<Scene> SceneManager::FindScene(const std::string& name)
{
	return _sceneTable[name];
}
