#include "pch.h"
#include "SceneManager.h"
#include "GameObject.h"
#include "TestScene.h"
#include "TestScene_jin.h"

std::unique_ptr<SceneManager> SceneManager::main = nullptr;

shared_ptr<Scene> SceneManager::AddScene(SceneType type, bool initExecute)
{

	shared_ptr<Scene> scene;

	switch (type)
	{
	case::SceneType::TestScene:
		scene = make_shared<TestScene>();
		break;
	case::SceneType::TestScene2:
		scene = make_shared<TestScene_jin>();
		break;
	default:
		break;
	}

	scene->InitGuid();
	scene->_type = type;
	_sceneTable[type] = scene;

	if(_currentScene == nullptr)
		_currentScene = scene;

	if(initExecute)
		scene->Init();

	return scene;
}

void SceneManager::ChangeScene(const shared_ptr<Scene>& prevScene, const shared_ptr<Scene>& nextScene)
{
	auto currentScene = prevScene;

	std::vector<std::shared_ptr<GameObject>> dontObj;

	if (currentScene != nullptr)
	{
		for (auto& obj : prevScene->_dont_destroy_gameObjects)
			if (!obj->IsDestroy())
				obj->GetChildsAll(dontObj);
	}

	for (auto& obj : dontObj)
	{
		nextScene->AddGameObject(obj);
		nextScene->_dont_destroy_gameObjects.push_back(obj);
		if(currentScene != nullptr)
			currentScene->RemoveGameObject(obj);
	}
	if(currentScene != nullptr)
		currentScene->_dont_destroy_gameObjects.clear();
	_currentScene = nextScene;
}

std::shared_ptr<Scene> SceneManager::FindScene(SceneType type)
{
	return _sceneTable[type];
}

void SceneManager::Reload()
{
	//SceneManager::GetCurrentScene()->_type
	auto prevScene = GetCurrentScene();
	auto nextScene = AddScene(prevScene->_type, false);
	ChangeScene(prevScene, nextScene);
	prevScene->Release();
	nextScene->Init();
}
