#include "pch.h"
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
#include "Profiler.h"
#include "ComputeManager.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
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
	_finalDefferedMaterial = std::make_shared<Material>();
	_finalDefferedMaterial->SetShader(ResourceManager::main->Get<Shader>(L"finalShader"));
    _finalDefferedMaterial->SetPass(RENDER_PASS::Deferred);
}

void Scene::Update()
{
    


    Profiler::Set("Logic_Start");
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
    Profiler::Fin();

    Profiler::Set("Logic_Update1");
    for (auto& gameObject : _gameObjects)
	    if(gameObject->GetType() == GameObjectType::Dynamic)
            gameObject->Update();
    Profiler::Fin();

    Profiler::Set("Logic_Update2");
    for (auto& gameObject : _gameObjects)
        if(gameObject->GetType() == GameObjectType::Dynamic)
            gameObject->Update2();
    Profiler::Fin();

    Profiler::Set("Logic_ColliderManager");
    ColliderManager::main->Update();
    Profiler::Fin();
}

void Scene::RenderBegin()
{
    for (auto& ele : _passObjects)
        ele.clear();

    Profiler::Set("Render_RenderBegin");
    for (auto& gameObject : _gameObjects)
    	gameObject->RenderBegin();

    Profiler::Fin();
    Gizmo::main->RenderBegin();
}

void Scene::Rendering()
{
    GlobalSetting();

    auto& cmdList = Core::main->GetCmdList();
    Core::main->GetRenderTarget()->ClearDepth();

    ShadowPass(cmdList);
    Profiler::Set("PASS : Deferred", BlockTag::GPU);
    DeferredPass(cmdList);
    FinalRender(cmdList);
    Profiler::Fin();

    Core::main->CopyDepthTexture(Core::main->GetDSReadTexture(), Core::main->GetRenderTarget()->GetDSTexture());
    Core::main->GetDSReadTexture()->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	//
    Profiler::Set("PASS : Forward", BlockTag::GPU);
    ForwardPass(cmdList);
    Profiler::Fin();

    Core::main->CopyDepthTexture(Core::main->GetDSReadTexture(), Core::main->GetRenderTarget()->GetDSTexture());
    Core::main->GetDSReadTexture()->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);


    //
    Profiler::Set("PASS : Transparent", BlockTag::GPU);
    TransparentPass(cmdList); // Position,
    Profiler::Fin();

    Profiler::Set("PASS : Compute", BlockTag::GPU);
    ComputePass(cmdList);
    Profiler::Fin();

    Profiler::Set("PASS : UI", BlockTag::GPU);
    UiPass(cmdList);
    Profiler::Fin();

}


void Scene::TransparentPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
    { // Forward
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Transparent)];

		std::vector<RenderObjectStrucutre> vec;
        vec.reserve(2048);
        for (auto& [shader, vec2] : targets)
            vec.insert(vec.end(), vec2.begin(), vec2.end());

    	Vector3 cameraPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
        Vector3 cameraDir = CameraManager::main->GetActiveCamera()->GetCameraLook();

        auto tangentDistanceSquared = [&](const Vector3& center) -> float {
            Vector3 offset = center - cameraPos;
            float projection = offset.Dot(cameraDir);
            return offset.LengthSquared() - projection * projection;
        };

        std::ranges::sort(vec, [&](const RenderObjectStrucutre& a, const RenderObjectStrucutre& b) {
            return tangentDistanceSquared(a.renderer->_bound.Center) < tangentDistanceSquared(b.renderer->_bound.Center);
        });

        Shader* prevShader = nullptr;
        for (auto& ele : vec)
        {
            Shader* shader = ele.material->GetShader().get();
            if (shader != nullptr && shader != prevShader)
				cmdList->SetPipelineState(shader->_pipelineState.Get());

        	g_debug_forward_count++;

            if (ele.renderer->IsCulling() == true)
            {
                if (CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound()) == false)
                {
                    g_debug_forward_culling_count++;
                    continue;
                }
            }

            SettingPrevData(ele, RENDER_PASS::PASS::Transparent);
            InstancingManager::main->RenderNoInstancing(ele);

			prevShader = shader;
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
                g_debug_forward_count++;

                if(ele.renderer->IsCulling() == true)
                {
                    if(CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound())==false)
                    {
                        g_debug_forward_culling_count++;
                        continue;
                    }
                }

                SettingPrevData(ele, RENDER_PASS::PASS::Forward);
                if (ele.renderer->isInstancing() == false)
                {
                    InstancingManager::main->RenderNoInstancing(ele);
                }
                else
                {
                    InstancingManager::main->AddObject(ele);
                }
            }

            InstancingManager::main->Render();
        }

    }
}

void Scene::DeferredPass(ComPtr<ID3D12GraphicsCommandList> & cmdList)
{
	Core::main->GetGBuffer()->RenderBegin();

    { // Deferred
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Deferred)];

        for (auto& [shader, vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());
         
            for (auto& ele : vec)
            {
                g_debug_deferred_count++;

                   if(ele.renderer->IsCulling() == true)
                   {
                       if(CameraManager::main->GetActiveCamera()->IsInFrustum(ele.renderer->GetBound())==false)
                       {
                           g_debug_deferred_culling_count++;
                           continue;
                       }
                   }

                   SettingPrevData(ele, RENDER_PASS::PASS::Deferred);

                   if (ele.renderer->isInstancing() == false)
                   {
                       InstancingManager::main->RenderNoInstancing(ele);
                   }
                   else
                   {    //동적인스턴싱이면 1개짜리 객체라도 스터럭쳐버퍼로 transform 데이터넣어줌.
                       InstancingManager::main->AddObject(ele);
                   }           
            }

        
           InstancingManager::main->Render();

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

            for(auto& renderStructure : vec)
            {
                auto& [material, mesh, target] = renderStructure;
                SettingPrevData(renderStructure, RENDER_PASS::PASS::Shadow);
                target->Rendering(nullptr,mesh);
            }
        }
    }
}

void Scene::UiPass(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    {
        auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::UI)];

        for (auto& [shader, vec] : targets)
        {
            cmdList->SetPipelineState(shader->_pipelineState.Get());

            for (auto& renderStructure : vec)
            {
                auto& [material, mesh, target] = renderStructure;
                SettingPrevData(renderStructure, RENDER_PASS::PASS::UI);
                target->Rendering(material, mesh);
            }
        }
    }
};

void Scene::FinalRender(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    Core::main->GetRenderTarget()->RenderBegin();

    auto mesh = ResourceManager::main->Get<Mesh>(L"finalMesh");
    auto shader = _finalDefferedMaterial->GetShader();

    cmdList->SetPipelineState(shader->_pipelineState.Get());

    RenderObjectStrucutre ROS = { _finalDefferedMaterial.get(), mesh.get(), nullptr };
    SettingPrevData(ROS, RENDER_PASS::PASS::Deferred);

    _finalDefferedMaterial->SetData();
    mesh->Redner();
}

void Scene::ComputePass(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
    //후처리작업진행할거임.
    ComputeManager::main->Dispatch(cmdList);
}


void Scene::GlobalSetting()
{
    auto& cmdList = Core::main->GetCmdList();

    CameraControll();
    //cout << CameraManager::main->GetActiveCamera()->GetCameraPos().y << endl;

    _globalParam.window_size = vec2(WINDOW_WIDTH,WINDOW_HEIGHT);
    _globalParam.Time = Time::main->GetTime();
    auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::GlobalParam)->Alloc(1);
    memcpy(CbufferContainer->ptr,(void*)&_globalParam,sizeof(GlobalParam));

    cmdList->SetGraphicsRootConstantBufferView(0,CbufferContainer->GPUAdress);
    cmdList->SetComputeRootConstantBufferView(0, CbufferContainer->GPUAdress);


    auto& table = Core::main->GetBufferManager()->GetTable();
    tableContainer container = Core::main->GetBufferManager()->GetTable()->Alloc(1);
    table->CopyHandle(container.CPUHandle, Core::main->GetDSReadTexture()->GetSRVCpuHandle(), 0);
    cmdList->SetGraphicsRootDescriptorTable(GLOBAL_SRV_DEPTH_INDEX, container.GPUHandle);
}

void Scene::SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass)
{
    data.material->SetTexture("_BakedGIMap", ResourceManager::main->_bakedGITexture);
}

void Scene::DebugRendering()
{

    auto& cmdList = Core::main->GetCmdList();



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
    Gizmo::main->Clear();
}

void Scene::RenderEnd()
{
  
}

void Scene::Finish()
{
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

void Scene::CameraControll()
{
	static CameraType type = CameraType::DebugCamera;

	if (Input::main->GetKeyDown(KeyCode::F1))
	{
		type = CameraType::DebugCamera;
        Input::main->SetMouseLock(false);
	}

    if (Input::main->GetKeyDown(KeyCode::F2))
    {
		type = CameraType::ComponentCamera;
    }

    if (Input::main->GetKeyDown(KeyCode::F3))
    {
        type = CameraType::SeaCamera;
    }

	CameraManager::main->Setting(type);
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
