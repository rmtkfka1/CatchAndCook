#include "pch.h"
#include "Terrain.h"
#include "GameObject.h"
#include "TerrainManager.h"
#include "Transform.h"

Terrain::Terrain()
{
	
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

	TerrainManager::main->PushTerrain(static_pointer_cast<Terrain>(shared_from_this()));

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

void Terrain::CollisionBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other)
{

}

void Terrain::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);

}

bool Terrain::IsExecuteAble()
{
	return Component::IsExecuteAble();
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

    // float 배열 할당
    _rawData = new float*[height];
    for(int z = 0; z < height; ++z)
    {
        _rawData[z] = new float[width];
    }

    // 파일 열기
    std::ifstream file(rawData,std::ios::binary);
    if(!file)
    {
        std::wcerr << L"Failed to open raw file: " << rawData << std::endl;
        return;
    }


    WORD* tempRow = new WORD[width];

    // 데이터 읽기
    for(int z = 0; z < height; ++z)
    {
        file.read(reinterpret_cast<char*>(tempRow),width * sizeof(WORD));

        for(int x = 0; x < width; ++x)
        {
            _rawData[z][x] = static_cast<float>(tempRow[x]) / 65535.0f * 1000.0f;
        }
    }

    delete[] tempRow;  
    file.close();

   
}


float Terrain::TerrainGetHeight(float x,float z)
{
    vec3 terrainOrigin = GetOwner()->_transform->GetLocalPosition();

    float tempX = x + _gridSize.x / 2;
    float tempZ = z + _gridSize.y / 2 ;

    Matrix mat;
    GetOwner()->_transform->GetLocalToWorldMatrix(mat);

    vec3 Coord= vec3::Transform({tempX,0,tempZ},mat.Invert());

    if(Coord.x < 0 || Coord.x >= _gridSize.x || Coord.z < 0 || Coord.z >= _gridSize.y)
    {
        return 0;
    }

    float ratioX = (Coord.x)/ _gridSize.x;
    float ratioZ = (Coord.z) / _gridSize.y;

    int ix = static_cast<int>(ratioX * _size.x);
    int iz = static_cast<int>(ratioZ * _size.y);

    return terrainOrigin.y + _rawData[iz][ix] + 6.0f; //temp는 임시적
}