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

 
    std::ifstream file("../Resources/Textures/sea/sea_color.bin", std::ios::binary);
    if (file)
    {
        file.read(reinterpret_cast<char*>(&_seaParam.seaBaseColor), sizeof(_seaParam.seaBaseColor));
        file.read(reinterpret_cast<char*>(&_seaParam.seaShallowColor), sizeof(_seaParam.seaShallowColor));
        file.read(reinterpret_cast<char*>(&_seaParam.blendingFact), sizeof(_seaParam.blendingFact));
        file.read(reinterpret_cast<char*>(&_seaParam.diffuseColor), sizeof(_seaParam.diffuseColor));
        file.read(reinterpret_cast<char*>(&_seaParam.specularPower), sizeof(_seaParam.specularPower));
        file.read(reinterpret_cast<char*>(&_seaParam.sun_dir), sizeof(_seaParam.sun_dir));
        file.read(reinterpret_cast<char*>(&_seaParam.env_power), sizeof(_seaParam.env_power));

        file.close();
    }
    else
    {
        cout << "파도색깔 bin 없음" << "\n";
    };
	
	std::ifstream in("../Resources/Textures/sea/sea_move.bin", std::ios::binary);
	if (in.is_open())
	{

		in.read(reinterpret_cast<char*>(&_seaParam.wave_count), sizeof(_seaParam.wave_count));

		for (int i = 0; i < _seaParam.wave_count; i++)
		{
			in.read(reinterpret_cast<char*>(&_seaParam.waves[i]), sizeof(Wave));
		}

		in.close();
	}
    else
    {
        cout << "파도움직임 bin 없음" << "\n";
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
