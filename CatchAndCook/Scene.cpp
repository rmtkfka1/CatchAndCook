#include "pch.h"
#include "Scene.h"

#include "Core.h"
#include "GameObject.h"
#include "RendererBase.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Camera.h"
#include "CameraManager.h"

void Scene::AddGameObject(const std::shared_ptr<GameObject> gameObject)
{
	gameObject->SetScene(GetCast<Scene>());
	_gameObjects.push_back(gameObject);
}

void Scene::Init()
{

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


    for (auto& ele : _passObjects)
    {
        ele.clear();
    }

}

void Scene::RenderBegin()
{
    for (auto& gameObject : _gameObjects)
		gameObject->RenderBegin();
    
}

void Scene::Rendering()
{
    auto& cmdList = Core::main->GetCmdList();

    CameraManager::main->SetActiveCamera(CameraType::ThirdPersonCamera);
    CameraManager::main->GetActiveCamera()->Update();
    CameraManager::main->GetActiveCamera()->PushData();
    CameraManager::main->GetActiveCamera()->SetData();

    _globalParam.window_size = vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
    _globalParam.Time = Time::main->GetTime();
    auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::GlobalParam)->Alloc(1);
    memcpy(CbufferContainer->ptr, (void*)&_globalParam, sizeof(GlobalParam));
    cmdList->SetGraphicsRootConstantBufferView(0, CbufferContainer->GPUAdress);


    { // Shadow
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

        for (auto& [material, mesh,target] : targets)
        {
            target->Rendering(nullptr, mesh);
        }
    }

    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deffered)];

        for (auto& [material, mesh, target] : targets)
        {
            cmdList->SetPipelineState(material->GetShader()->_pipelineState.Get());
            target->Rendering(material, mesh);
        }
    }

	{ // forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

        for (auto& [material, mesh, target] : targets)
        {
            cmdList->SetPipelineState(material->GetShader()->_pipelineState.Get());
            target->Rendering(material, mesh);
        }
	}

    {  //UI
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI)];

        for (auto& [material, mesh, target] : targets)
        {
            target->Rendering(nullptr, mesh);
        }
    }

  
}

void Scene::DebugRendering()
{
    { // Shadow
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

        for (auto& [material, mesh, target] : targets)
        {
            target->DebugRendering();
        }
    }

    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deffered)];

        for (auto& [material, mesh, target] : targets)
        {
            target->DebugRendering();
        }
    }

    { // forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

        for (auto& [material, mesh, target] : targets)
        {
            target->DebugRendering();
        }
    }
}

void Scene::RenderEnd()
{
    

}

void Scene::Finish()
{

    while (_destroyQueue.empty() == false)
    {
         auto& gameObject =_destroyQueue.front();
        _destroyQueue.pop();

        auto it =std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);

        if (it != _gameObjects.end())
        {
            gameObject->Destroy();
            _gameObjects.erase(it);
        }       
    }

   

  
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

void Scene::AddDestroyQueue(const std::shared_ptr<GameObject>& gameObject)
{
	_destroyQueue.push(gameObject);
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

void Scene::AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
    {
        if (RENDER_PASS::HasFlag(material->GetPass(), RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i].emplace_back(material, mesh, renderBase);

        }
    }
}

void Scene::AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
        if (RENDER_PASS::HasFlag(pass, RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i].emplace_back(nullptr, mesh, renderBase);
        }
}

void Scene::Release()
{

}
