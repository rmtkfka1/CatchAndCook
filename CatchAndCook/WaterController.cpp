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
	vector<wstring> paths;

    paths.resize(120);
    
    for (int i = 0; i <= 119; ++i)
    {
        paths[i]= (L"../Resources/Textures/sea/" + to_wstring(i) + L".png");
    }

    _textures = make_shared<Texture>();
	_textures->Init(paths);

    _bump = make_shared<Texture>();
    _bump->Init(L"../Resources/Textures/sea/tt.jpg");

	_dudv = make_shared<Texture>();
	_dudv->Init(L"../Resources/Textures/sea/dudv2.png");

	ImguiManager::main->_seaParam = &_seaParam;


	{
		_seaParam.waves[0].amplitude = 2.5f;
		_seaParam.waves[0].wavelength = 150.f;
		_seaParam.waves[0].speed = 1.0f;
		_seaParam.waves[0].steepness = 0.5f;
		vec2 dir = vec2(0.4f, 0.2f);
		dir.Normalize();
		_seaParam.waves[0].direction = dir;
	}

	{
		_seaParam.waves[1].amplitude = 1.5f;
		_seaParam.waves[1].wavelength = 200.f;
		_seaParam.waves[1].speed = 0.4f;
		_seaParam.waves[1].steepness = 0.4f;
		vec2 dir = vec2(0.7f, -1.0f);
		dir.Normalize();
		_seaParam.waves[1].direction = dir;
	}
	{
		_seaParam.waves[2].amplitude = 1.0f;
		_seaParam.waves[2].wavelength = 150.f;
		_seaParam.waves[2].speed = 3.0f;
		_seaParam.waves[2].steepness = 0.3f;
		vec2 dir = vec2(0.5f, 0.7f);
		dir.Normalize();
		_seaParam.waves[2].direction = dir;
	}
	

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
    material->SetHandle("_bumpMap", _textures->GetSRVCpuHandle());
	material->SetHandle("_bumpMap2", _bump->GetSRVCpuHandle());
    material->SetHandle("_dudv", _dudv->GetSRVCpuHandle());

	_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SeaParam)->Alloc(1);

	memcpy(_cbufferContainer->ptr, (void*)&_seaParam, sizeof(SeaParam));

	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SeaParam"), _cbufferContainer->GPUAdress);

}
