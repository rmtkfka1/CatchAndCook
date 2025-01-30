#include "pch.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TestScene.h"
#include "TestScene_jin.h"

std::unique_ptr<SceneManager> SceneManager::main = nullptr;

shared_ptr<Scene> SceneManager::AddScene(SceneType type)
{
	if (_sceneTable.find(type) != _sceneTable.end())
	{
		assert(false && "Scene name already exists!");
	}

	shared_ptr<Scene> scene;

	switch (type)
	{
	case::SceneType::TestScene:
		scene = make_shared<TestScene>();
		scene->InitGuid();
		_sceneTable[SceneType::TestScene] = scene;
		break;
	case::SceneType::TestScene2:
		scene = make_shared<TestScene_jin>();
		scene->InitGuid();
		_sceneTable[SceneType::TestScene2] = scene;
		break;

	default:
		break;
	}

	if(_currentScene == nullptr)
		_currentScene = scene;

	scene->Init();
	return scene;
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

std::shared_ptr<Scene> SceneManager::FindScene(SceneType type)
{
	return _sceneTable[type];
}
