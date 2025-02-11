#include "pch.h"
#include "Terrain.h"
#include "GameObject.h"
#include "TerrainManager.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include <algorithm>
Terrain::Terrain()
{
	
}

Terrain::~Terrain()
{
   
}

void Terrain::Init()
{

}

void Terrain::Start()
{
	Component::Start();

	if(auto &renderer =GetOwner()->GetRenderer())
	{
		renderer->AddSetter(static_pointer_cast<Terrain>(shared_from_this()));

		if(_gridMesh == nullptr)
		{
            assert(false);
		}

		dynamic_pointer_cast<MeshRenderer>(renderer)->AddMesh(_gridMesh);
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
	if(auto &renderer =GetOwner()->GetRenderer())
	{
        renderer->RemoveSetters(static_pointer_cast<Terrain>(shared_from_this()));
	}
}

void Terrain::PushData()
{

}

void Terrain::SetData(Material * material)
{
	material->SetHandle("heightMap",_heightMap->GetSRVCpuHandle());
}

void Terrain::SetHeightMap(const std::wstring &rawData,const std::wstring &pngData , float scale)
{
 
    _heightMap = make_shared<Texture>();
    _heightMap->Init(pngData);

    _heightMapX = static_cast<int>(_heightMap->GetResource()->GetDesc().Width);
    _heightMapZ = static_cast<int>(_heightMap->GetResource()->GetDesc().Height);

	_gridXsize = _heightMapX * scale;
	_gridZsize = _heightMapZ * scale;

    _gridMesh = GeoMetryHelper::LoadGripMesh(_heightMapX,_heightMapZ,CellsPerPatch,CellsPerPatch);
    _gridMesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	LoadTerrain(rawData);
    //Smooth();
   
}

float Terrain::TerrainGetHeight(float x,float z,float offset)
{
    vec3 terrainOrigin = GetOwner()->_transform->GetLocalPosition();

    float tempX = x + _gridXsize * 0.5f;
    float tempZ = z + _gridZsize * 0.5f;

    tempX-= terrainOrigin.x;
    tempZ-= terrainOrigin.z;

    if(tempX< 0 || tempX >= _gridXsize-1 || tempZ < 0 || tempZ >= _gridZsize-1)
    {
        return 0;
    }

    float heightMapScaleX = _heightMapX / _gridXsize;
    float heightMapScaleZ = _heightMapZ / _gridZsize;

    float fx = tempX * heightMapScaleX;
    float fz = tempZ * heightMapScaleZ;

    int ix = static_cast<int>(fx);
    int iz = static_cast<int>(fz);

    // 경계를 넘어가는 경우 방지
    int ix1 = std::min(ix + 1,(int)(_heightMapX - 1));
    int iz1 = std::min(iz + 1,(int)(_heightMapZ - 1));

    float hx0z0 = _heightMapData[iz][ix];
    float hx1z0 = _heightMapData[iz][ix1];
    float hx0z1 = _heightMapData[iz1][ix];
    float hx1z1 = _heightMapData[iz1][ix1];

    float tx = fx - ix;
    float tz = fz - iz;

    float h0 = hx0z0 * (1 - tx) + hx1z0 * tx;  // x 방향 보간
    float h1 = hx0z1 * (1 - tx) + hx1z1 * tx;  // x 방향 보간
    float finalHeight = h0 * (1 - tz) + h1 * tz; // z 방향 보간

    return terrainOrigin.y + finalHeight + offset;
}



void Terrain::LoadTerrain(const std::wstring &rawData)
{
    _heightMapData.resize(_heightMapZ,vector<float>(_heightMapX,0.0f)); 


    std::ifstream file(rawData,std::ios::binary);
    if(!file)
    {
        std::wcerr << L"Failed to open raw file: " << rawData << std::endl;
        return;
    }

    std::vector<WORD> tempRow((int)_heightMapX);

    for(int z = 0; z < _heightMapZ; ++z)
    {
        if(!file.read(reinterpret_cast<char*>(tempRow.data()),_heightMapX * sizeof(WORD)))
        {
            std::wcerr << L"Failed to read raw data from file: " << rawData << std::endl;
            return;
        }

        for(int x = 0; x < _heightMapX; ++x)
        {
            _heightMapData[z][x] = static_cast<float>(tempRow[x]) / 65535.0f * 1000.0f;
        }
    }

    file.close();
}

void Terrain::Smooth()
{
    vector<vector<float>> temp;

	temp = _heightMapData;

    for(uint32 i = 0; i < _heightMapZ; ++i)
    {
        for(uint32 j = 0; j <_heightMapX; ++j)
        {
            temp[i][j] = Average(i,j);
        }
    }

    _heightMapData = temp;
}

bool Terrain::InBounds(int32 i,int32 j)
{
    return
        i >= 0 && i < (int32)_heightMapZ &&
        j >= 0 && j < (int32)_heightMapX;
}

float Terrain::Average(int32 i,int32 j)
{
    float avg = 0.0f;
    float num = 0.0f;

    for(int32 z = i - 1; z <= i + 1; ++z)
    {
        for(int32 x = j - 1; x <= j + 1; ++x)
        {
            if(InBounds(z,x))
            {
                avg += _heightMapData[z][x];
                num += 1.0f;
            }
        }
    }

    return avg / num;
}
