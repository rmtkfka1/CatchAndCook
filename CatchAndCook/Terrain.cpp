#include "pch.h"
#include "Terrain.h"
#include "GameObject.h"

Terrain::Terrain(int32 size_x,int32 size_y,int32 division_x,int32 division_y)
{
	_mesh=GeoMetryHelper::LoadGripMesh(size_x,size_y,division_x,division_y);
}

Terrain::~Terrain()
{
    if(_rawData)
    {
        for(int y = 0; y < static_cast<int>(_size.y); ++y)
        {
            delete[] _rawData[y];
        }
        delete[] _rawData;
        _rawData = nullptr;
    }
}

void Terrain::Init()
{

}

void Terrain::Start()
{
	Component::Start();

	if(GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->AddSetter(static_pointer_cast<Terrain>(shared_from_this()));
	}
}

void Terrain::Update()
{

}

void Terrain::Update2()
{

}

void Terrain::Enable()
{

}

void Terrain::Disable()
{

}

void Terrain::RenderBegin()
{

}

void Terrain::Collision(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other)
{

}

void Terrain::SetDestroy()
{

}

void Terrain::Destroy()
{
	if(GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->RemoveSetters(static_pointer_cast<Terrain>(shared_from_this()));
	}
}

void Terrain::PushData()
{

}

void Terrain::SetData(Material * material)
{
	material->SetHandle("heightMap",_heightMap->GetSRVCpuHandle());
}

void Terrain::SetHeightMap(const std::wstring &rawData,const std::wstring &pngData)
{
    _heightMap = make_shared<Texture>();
    _heightMap->Init(pngData);

    int width = static_cast<int>(_heightMap->GetResource()->GetDesc().Width);
    int height = static_cast<int>(_heightMap->GetResource()->GetDesc().Height);

    _size = vec2(width,height);

    _rawData = new WORD*[height];
    for(int y = 0; y < height; ++y)
    {
        _rawData[y] = new WORD[width];
    }

    std::ifstream file(rawData,std::ios::binary);
    if(!file)
    {
        std::wcerr << L"Failed to open raw file: " << rawData << std::endl;
        return;
    }

    for(int y = 0; y < height; ++y)
    {
        file.read(reinterpret_cast<char*>(_rawData[y]),width * sizeof(WORD));
    }

    file.close();

    const int MaxHeight =1000;

    for(int y=0; y<height; ++y)
    {
        for(int x=0; x<width; ++x)
        {
            _rawData[y][x] = static_cast<float>(_rawData[y][x]) / 65535.0f * MaxHeight;
        };
    };

    std::cout << _rawData[0][0] << " ";

};


 