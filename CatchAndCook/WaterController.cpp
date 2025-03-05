#include "pch.h"
#include "WaterController.h"
#include "RendererBase.h"

WaterController::~WaterController()
{
}

bool WaterController::IsExecuteAble()
{
    return false;
}

void WaterController::Init()
{
    _textures.resize(120);
    
    for (int i = 0; i <= 119; ++i)
    {
		_textures[i] = make_shared<Texture>();
        _textures[i] =ResourceManager::main->Load<Texture>(L"waterNormal" + to_wstring(i), L"../Resources/Textures/sea/" + to_wstring(i) + L".png");
        
    }

    _textures[0] = ResourceManager::main->Load<Texture>(L"waterNormaltt", L"../Resources/Textures/sea/normalMap.png");

}

void WaterController::Start()
{
    if (auto& renderer = GetOwner()->_renderer)
    {
        renderer->AddCbufferSetter(static_pointer_cast<WaterController>(shared_from_this()));
    }
}

void WaterController::Update()
{
}

void WaterController::Update2()
{

}

void WaterController::Enable()
{
}

void WaterController::Disable()
{
}

void WaterController::RenderBegin()
{
}

void WaterController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void WaterController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void WaterController::SetDestroy()
{
}

void WaterController::Destroy()
{
    if (auto& renderer = GetOwner()->_renderer)
    {
        renderer->RemoveCbufferSetter(static_pointer_cast<WaterController>(shared_from_this()));
    }
}

void WaterController::SetData(Material* material)
{
    frameCount += Time::main->GetDeltaTime() * 10;


	if (frameCount >= 120)
		frameCount = 0;

    material->SetHandle("_bumpMap", _textures[0]->GetSRVCpuHandle());
}
