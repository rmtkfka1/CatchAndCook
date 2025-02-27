﻿#include "pch.h"
#include "Scene.h"
#include "Core.h"
#include "GameObject.h"
#include "RendererBase.h"
#include "RenderTarget.h"
#include "Shader.h"
#include "Camera.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "Mesh.h"
#include "InstancingManager.h"

void Scene::AddGameObject(const std::shared_ptr<GameObject>& gameObject)
{
	gameObject->SetScene(GetCast<Scene>());

    if (gameObject->GetType() == GameObjectType::Deactivate)
        _gameObjects_deactivate.push_back(gameObject);
    else
		_gameObjects.push_back(gameObject);
}

void Scene::Init()
{

}

void Scene::Update()
{
    while (!_changeTypeQueue.empty()) 
    {
        auto& current = _changeTypeQueue.front();
        {
            RemoveGameObject(current.first);
            current.first->SetType(current.second);
            AddGameObject(current.first);
        }
        _changeTypeQueue.pop();
    }

    while (!_startQueue.empty()) 
    {
        auto& current = _startQueue.front();
        current->Start();
        _startQueue.pop();
    }


    for (auto& gameObject : _gameObjects)
	    if(gameObject->GetType() == GameObjectType::Dynamic)
            gameObject->Update();
    for (auto& gameObject : _gameObjects)
        if(gameObject->GetType() == GameObjectType::Dynamic)
            gameObject->Update2();

    ColliderManager::main->Update();
}

void Scene::RenderBegin()
{
    for (auto& ele : _passObjects)
        ele.clear();

    for (auto& gameObject : _gameObjects)
    	gameObject->RenderBegin();

    Gizmo::main->RenderBegin();
}

void Scene::Rendering()
{
    GlobalSetting();

    auto& cmdList = Core::main->GetCmdList();

    Core::main->GetRenderTarget()->ClearDepth();

    //ShadowPass(cmdList);
    DefferedPass(cmdList);
    FinalRender(cmdList);
    ForwardPass(cmdList);
    TransparentPass(cmdList);
    UiPass(cmdList);

}

void Scene::UiPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
    {  //UI
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI)];

        for(auto& [shader,vec] : targets)
        {
            if(shader)
                cmdList->SetPipelineState(shader->_pipelineState.Get());

            for(auto& [material,mesh,target] : vec)
            {
                target->Rendering(material,mesh);
            }
        }
    }
}

void Scene::TransparentPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
    { // Transparent
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Transparent)];

        for(auto& [shader,vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());

            for(auto& [material,mesh,target] : vec)
            {
                target->Rendering(material,mesh);
            }
        }
    }
}

void Scene::ForwardPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{

    { // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

        for(auto& [shader,vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());

            for(auto& ele : vec)
            {
                if(ele.renderer->IsCulling() == true)
                {
                    Gizmo::Box(ele.renderer->GetBound());
                    if(CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound())==false)
                    {
                        continue;
                    }
                }
                if(ele.renderer->IsInstancing() == false)
                {
                    //여기서 CBuffer 넣어줘야
                    InstancingManager::main->AddObject(ele);
                    InstancingManager::main->Render();
                }
				InstancingManager::main->AddObject(ele);
            }

            InstancingManager::main->Render();
        }

        
    }
}

void Scene::DefferedPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
	Core::main->GetGBuffer()->RenderBegin();

    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deffered)];

        for(auto& [shader,vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());
            
            for(auto& [material,mesh,target] : vec)
            {
				if(target->IsCulling() == true)
				{
                    if(CameraManager::main->GetActiveCamera()->IsInFrustum(target->GetBound())==false)
                    {
                        continue;
                    }
				}

                target->Rendering(material,mesh);
            }
        }
    }

    Core::main->GetGBuffer()->RenderEnd();
}

void Scene::ShadowPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
    { // Shadow
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

        for(auto& [shader,vec] : targets)
        {
            cmdList->SetPipelineState(ResourceManager::main->Get<Shader>(L"Shadow")->_pipelineState.Get());

            for(auto& [material,mesh,target] : vec)
            {
                target->Rendering(nullptr,mesh);
            }
        }
    }
}

void Scene::FinalRender(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Core::main->GetRenderTarget()->RenderBegin();

    auto& mesh = ResourceManager::main->Get<Mesh>(L"finalMesh");
    auto& shader = ResourceManager::main->Get<Shader>(L"finalShader");

    cmdList->SetPipelineState(shader->_pipelineState.Get());
    mesh->Redner();
}

void Scene::GlobalSetting()
{
    auto& cmdList = Core::main->GetCmdList();

    CameraManager::main->SetActiveCamera(CameraType::ThirdPersonCamera);
    CameraManager::main->GetActiveCamera()->Update();
    CameraManager::main->GetActiveCamera()->PushData();
    CameraManager::main->GetActiveCamera()->SetData();

	/*cout << CameraManager::main->GetActiveCamera()->GetCameraLook().x << " " << CameraManager::main->GetActiveCamera()->GetCameraLook().y << " " << CameraManager::main->GetActiveCamera()->GetCameraLook().z << endl;*/

    _globalParam.window_size = vec2(WINDOW_WIDTH,WINDOW_HEIGHT);
    _globalParam.Time = Time::main->GetTime();
    auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::GlobalParam)->Alloc(1);
    memcpy(CbufferContainer->ptr,(void*)&_globalParam,sizeof(GlobalParam));

    cmdList->SetGraphicsRootConstantBufferView(0,CbufferContainer->GPUAdress);
}

void Scene::DebugRendering()
{

    auto& cmdList = Core::main->GetCmdList();


    { // Deffered
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deffered)];

        for(auto& [shader,vec] : targets)
        {
            for(auto& [material,mesh,target] : vec)
            {
                target->DebugRendering();
            }
        }
    }

    { // forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Forward)];

        for(auto& [shader,vec] : targets)
        {
            for(auto& [material,mesh,target] : vec)
            {
                target->DebugRendering();
            }
        }
    }

    { // forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Debug)];

        for(auto& [shader,vec] : targets)
        {
            for(auto& [material,mesh,target] : vec)
            {
                target->Rendering(material,mesh);
            }
        }
    }
}

void Scene::RenderEnd()
{
  
}

void Scene::Finish()
{
    Gizmo::main->Clear();
    Scene::ExecuteDestroyGameObjects();
    GameObject::ExecuteDestroyComponents();
}

void Scene::ExecuteDestroyGameObjects()
{
    while (_destroyQueue.empty() == false)
    {
        auto& gameObject = _destroyQueue.front();
        _destroyQueue.pop();

        auto it = std::find(_gameObjects.begin(), _gameObjects.end(), gameObject);

        if (it != _gameObjects.end())
        {
            gameObject->Destroy();
            _gameObjects.erase(it);
        }
        else
        {
            it = std::find(_gameObjects_deactivate.begin(), _gameObjects_deactivate.end(), gameObject);
            if (it != _gameObjects_deactivate.end())
            {
                gameObject->Destroy();
                _gameObjects_deactivate.erase(it);
            }
        }
    }
}


std::shared_ptr<GameObject> Scene::CreateGameObject(const std::wstring& name, GameObjectType type)
{
	auto gameObject = std::make_shared<GameObject>(name);
    gameObject->InitGuid();
    gameObject->SetType(type);
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
        return true;
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
    if (iter != this->_gameObjects.end())
		return *iter;

    iter = std::find_if(this->_gameObjects_deactivate.begin(), this->_gameObjects_deactivate.end(),
        [&](const std::shared_ptr<GameObject>& element)
        {
            if (!includeDestroy && element->IsDestroy())
                return false;
            return element->GetName() == name;
        });
    if (iter != this->_gameObjects_deactivate.end())
        return *iter;

    return nullptr;
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
    for (int i = 0; i < this->_gameObjects_deactivate.size(); i++)
    {
        auto& current = this->_gameObjects_deactivate[i];
        if (!includeDestroy && current->IsDestroy())
            continue;
        if (current->GetName() == name)
            vec.push_back(current);
    }
    return vec.size() - startSize;
}

void Scene::AddChangeTypeQueue(const std::shared_ptr<GameObject>& gameObject, GameObjectType type)
{
    _changeTypeQueue.push(std::make_pair(gameObject, type));
}

void Scene::AddStartQueue(const std::shared_ptr<GameObject>& gameObject)
{
    _startQueue.push(gameObject);
}

void Scene::AddRenderer(Material* material, Mesh* mesh, RendererBase* renderBase)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
    {
        if (RENDER_PASS::HasFlag(material->GetPass(), RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i][material->GetShader()].emplace_back(material,mesh,renderBase);

        }
    }
}

void Scene::AddRenderer(Mesh* mesh, RendererBase* renderBase, RENDER_PASS::PASS pass)
{
    for (int i = 0; i < RENDER_PASS::Count; i++)
        if (RENDER_PASS::HasFlag(pass, RENDER_PASS::PASS(1 << i)))
        {
            _passObjects[i][nullptr].emplace_back(nullptr,mesh,renderBase);
        }
}

void Scene::Release()
{
    for(auto& obj : _gameObjects)
        obj->SetDestroy();
    for(auto& obj : _gameObjects_deactivate)
        obj->SetDestroy();
    Scene::ExecuteDestroyGameObjects();
    GameObject::ExecuteDestroyComponents();

    Core::main->Fence();

    for(auto pass : _passObjects)
        pass.clear();
    _dont_destroy_gameObjects.clear();
    _gameObjects.clear();
    _gameObjects_deactivate.clear();

    while(!_destroyQueue.empty()) _destroyQueue.pop();
    while(!_destroyComponentQueue.empty()) _destroyComponentQueue.pop();
    while(!_startQueue.empty()) _startQueue.pop();
    while(!_changeTypeQueue.empty()) _changeTypeQueue.pop();
}
