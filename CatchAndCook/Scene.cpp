#include "pch.h"
#include "Scene.h"

#include "Core.h"
#include "GameObject.h"
#include "RendererBase.h"
#include "RenderTarget.h"
#include "Shader.h"

void Scene::AddGameObject(const std::shared_ptr<GameObject> gameObject)
{
	gameObject->SetScene(GetCast<Scene>());
	_gameObjects.push_back(gameObject);
}

void Scene::Init(const std::string& name)
{
	SetName(name);


}

void Scene::Update()
{
    for (auto& gameObject : _gameObjects)
    {
    	gameObject->Start();
	    gameObject->Update();
	}
    for (auto& gameObject : _gameObjects)
        gameObject->Update2();

    for (int i = 0; i < RENDER_PASS::Count; i++)
        _passObjects[i].clear();
}

void Scene::RenderBegin()
{
    for (auto& gameObject : _gameObjects)
		gameObject->RenderBegin();
    
}

void Scene::Rendering()
{
    auto& cmdList = Core::main->GetCmdList();
    { // Shadow
        auto& targets = _passObjects[RENDER_PASS::Shadow];

        //이 시점에 Set
        for (auto& target : targets)
        {
            target.second->Rendering(nullptr);
        }
    }

    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::Deffered];

        for (auto& target : targets)
        {
            cmdList->SetPipelineState(target.first->GetShader()->_pipelineState.Get());
            target.second->Rendering(target.first);
        }
    }

	{ // forward
        auto& targets = _passObjects[RENDER_PASS::Forward];

        for (auto& target : targets)
        {
            cmdList->SetPipelineState(target.first->GetShader()->_pipelineState.Get());
            target.second->Rendering(target.first);
        }
	}
}

void Scene::RenderEnd()
{

}

void Scene::Finish()
{
    for (auto& gameObject : _gameObjects)
        gameObject->Destroy();
    std::erase_if(_gameObjects, [](const std::shared_ptr<GameObject>& gameObject) {return gameObject->IsDestroy(); });
}


std::shared_ptr<GameObject> Scene::CreateGameObject(const std::wstring& name)
{
	auto gameObject = std::make_shared<GameObject>(name);
	AddGameObject(gameObject);
	gameObject->Init();
	return gameObject;
}


bool Scene::RemoveGameObject(const std::shared_ptr<GameObject>& gameObject)
{
    auto iter = std::find(this->_gameObjects.begin(), this->_gameObjects.end(), gameObject);
    if (iter != this->_gameObjects.end()) {
        this->_gameObjects.erase(iter);
        return true;
    }
    return false;
}

bool Scene::RemoveAtGameObject(int index)
{
    if (index >= 0 && index < this->_gameObjects.size()) {
        this->_gameObjects.erase(this->_gameObjects.begin() + index);
    }
    return false;
}




std::shared_ptr<GameObject> Scene::Find(const std::wstring& name, bool includeDestroy)
{
    auto iter = std::find_if(this->_gameObjects.begin(), this->_gameObjects.end(),
        [&](const std::shared_ptr<GameObject>& element)
        {
            if (!includeDestroy && element->IsDestroy())
                return false;
            return element->GetName() == name;
        });
    if (iter == this->_gameObjects.end())
        return nullptr;
    return *iter;
}

int Scene::Finds(const std::wstring& name, std::vector<std::shared_ptr<GameObject>>& vec, bool includeDestroy)
{
    int startSize = vec.size();
    for (int i = 0; i < this->_gameObjects.size(); i++)
    {
        auto& current = this->_gameObjects[i];
        if (!includeDestroy && current->IsDestroy())
            continue;
        if (current->GetName() == name)
            vec.push_back(current);
    }
    return vec.size() - startSize;
}

void Scene::AddRenderObject(std::pair<std::shared_ptr<Material>, RendererBase*> data)
{
    for (int i=0;i<RENDER_PASS::Count;i++)
		if (RENDER_PASS::HasFlag(data.first->pass, RENDER_PASS::PASS(1 << i)))
            _passObjects[i].push_back(data);
}

void Scene::AddRenderObject(RendererBase* data, RENDER_PASS::PASS pass)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
        if (RENDER_PASS::HasFlag(pass, RENDER_PASS::PASS(1 << i)))
            _passObjects[i].push_back(std::make_pair(nullptr, data));
}

void Scene::Release()
{

}
