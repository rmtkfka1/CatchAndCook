#include "pch.h"
#include "Terrain.h"
#include "GameObject.h"
#include "TerrainManager.h"
#include "Transform.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include <algorithm>

#include "Camera.h"
#include "CameraManager.h"

Terrain::Terrain()
{
	
}

Terrain::~Terrain()
{
   
}

void Terrain::Init()
{
    GetOwner()->AddComponent<MeshRenderer>();
}

void Terrain::Start()
{
	Component::Start();

    ShaderInfo info;
    info._zTest = true;
    info._stencilTest = false;
    info.cullingType = CullingType::BACK;
    info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

    shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"TerrainTest",L"Terrain.hlsl", GeoMetryProp,
    ShaderArg{{{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}}},info);
    shader->SetInjector({BufferType::TerrainDetailsParam});
    
    _material->SetPropertyVector("fieldSize",Vector4(_fieldSize));
    _material->SetShader(shader);
    _material->SetPass(RENDER_PASS::Forward);
    auto meshRenderer = GetOwner()->GetComponent<MeshRenderer>();
    meshRenderer->AddMaterials({_material});

	if(auto renderer = GetOwner()->GetRenderer())
	{
		renderer->AddSetter(static_pointer_cast<Terrain>(shared_from_this()));
		if(_gridMesh == nullptr)
            assert(false);
		dynamic_pointer_cast<MeshRenderer>(renderer)->AddMesh(_gridMesh);
	}



    _instanceBuffers.resize(_instances.size());
	for(int i = 0; i<_instances.size();i++)
	{
        auto instanceBuffer = Core::main->GetBufferManager()->GetInstanceBufferPool(BufferType::TransformInstanceParam)->Alloc();
        memcpy(instanceBuffer->ptr,_instanceDatas[i].data(), _instanceDatas[i].size() * sizeof(Instance_Transform));
        instanceBuffer->SetIndex(_instanceDatas[i].size(),sizeof(Instance_Transform));
		//instanceBuffer->writeOffset = _instanceDatas[i].size() * sizeof(Instance_Transform);
        _instanceBuffers[i] = instanceBuffer;

        std::vector<std::shared_ptr<MeshRenderer>> renderers;
        _instances[i].lock()->GetComponentsWithChilds<MeshRenderer>(renderers);

		for(auto& renderer : renderers)
		{
			renderer->SetInstance(instanceBuffer);
            for(auto& material : renderer->GetMaterials())
                material->SetShader(ResourceManager::main->Get<Shader>(L"DefaultForward_Instanced"));
		}
	}



	TerrainManager::main->PushTerrain(static_pointer_cast<Terrain>(shared_from_this()));

}

void Terrain::Update()
{
    Vector3 cameraPos = Vector3(CameraManager::main->GetActiveCamera()->GetCameraPos().x,0,CameraManager::main->GetActiveCamera()->GetCameraPos().z);
    if(false)
    for(int i=0;i<_instanceDatas.size();++i)
    {
        _instanceBuffers[i]->Clear();
        for(int j=0;j<_instanceDatas[i].size();j++)
        {
            if(Vector3::Distance(Vector3(_instanceDatas[i][j].worldPosition.x, 0,_instanceDatas[i][j].worldPosition.z), cameraPos) < 10)
            {
                _instanceBuffers[i]->AddData(_instanceDatas[i][j]);
            }
        }
    }
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
	if(auto renderer =GetOwner()->GetRenderer())
	{
        renderer->RemoveSetters(static_pointer_cast<Terrain>(shared_from_this()));
	}
}

void Terrain::PushData()
{

}

void Terrain::SetData(Material* material)
{
	material->SetHandle("heightMap", _heightTexture->GetSRVCpuHandle());
}

void Terrain::SetHeightMap(const std::wstring &rawPath,const std::wstring &pngPath, const vec2& rawSize, const vec3& fieldSize)
{
    _heightTexture = ResourceManager::main->Load<Texture>(pngPath, pngPath, TextureType::Texture2D, false);

    _heightTextureSize = Vector2(static_cast<int>(_heightTexture->GetResource()->GetDesc().Width),
								static_cast<int>(_heightTexture->GetResource()->GetDesc().Height));
    _heightRawSize = rawSize;
    _fieldSize = fieldSize;

    _gridMesh = GeoMetryHelper::LoadGripMesh(_fieldSize.x,_fieldSize.z,CellsPerPatch,CellsPerPatch);
    _gridMesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	LoadTerrain(rawPath);
}

float Terrain::GetHeight(const Vector2& heightMapPosition) const
{
    const float fx = heightMapPosition.x;
    const float fz = heightMapPosition.y;
    if(fx < 0 || fx > _heightRawSize.x || fz < 0 || fz > _heightRawSize.y)
        return 0;

    const int ix = static_cast<int>(fx);
    const int iz = static_cast<int>(fz);

    // 경계를 넘어가는 경우 방지
    int ix1 = std::min(ix + 1,static_cast<int>(_heightRawSize.x - 1));
    int iz1 = std::min(iz + 1,static_cast<int>(_heightRawSize.y - 1));

    float hx0z0 = _heightRawMapData[iz][ix];
    float hx1z0 = _heightRawMapData[iz][ix1];
    float hx0z1 = _heightRawMapData[iz1][ix];
    float hx1z1 = _heightRawMapData[iz1][ix1];

    float tx = fx - ix;
    float tz = fz - iz;

    float h0 = hx0z0 * (1 - tx) + hx1z0 * tx;  // x 방향 보간
    float h1 = hx0z1 * (1 - tx) + hx1z1 * tx;  // x 방향 보간
    float finalHeight = h0 * (1 - tz) + h1 * tz; // z 방향 보간

    return finalHeight;
}

float Terrain::GetLocalHeight(const Vector3& localPosition) // float x,float z
{
    Vector3 terrainOrigin = GetOwner()->_transform->GetLocalPosition();

    //float tempX = x + _fieldSize.x * 0.5f;
    //float tempZ = z + _fieldSize.z * 0.5f;

    float tempX = localPosition.x - terrainOrigin.x;
    float tempZ = localPosition.z - terrainOrigin.z;
    /*tempX -= terrainOrigin.x;
    tempZ -= terrainOrigin.z;*/

    // 0~1범위로 축소시켰다가 -> 하이트맵으로 변환한다는 개념으로
    float finalH = GetHeight(Vector2((tempX / _fieldSize.x) * _heightRawSize.x, (tempZ / _fieldSize.z) * _heightRawSize.y));
    return terrainOrigin.y + finalH;
}

float Terrain::GetWorldHeight(const Vector3& worldPosition)
{
    Matrix matrix;
    Matrix invMatrix;
    GetOwner()->_transform->GetLocalToWorldMatrix_BottomUp(matrix);
    matrix.Invert(invMatrix);
    Vector3 localPosition;
    Vector3::Transform(worldPosition, invMatrix,localPosition);
    float tempX = localPosition.x;
    float tempZ = localPosition.z;
    float finalH = GetHeight(Vector2((tempX / _fieldSize.x) * _heightRawSize.x,(tempZ / _fieldSize.z) * _heightRawSize.y));
    
    return matrix.Translation().y + finalH;
}


void Terrain::LoadTerrain(const std::wstring &rawData)
{
    _heightRawMapData.resize(_heightRawSize.y,vector(_heightRawSize.x, 0.0f)); 


    std::ifstream file(rawData,std::ios::binary);
    if(!file) {
        std::wcerr << L"Failed to open raw file: " << rawData << std::endl;
        return;
    }

    std::vector<uint16_t> tempRow(static_cast<int>(_heightRawSize.x));

    for(int z = 0; z < _heightRawSize.y; ++z)
    {
        if(!file.read(reinterpret_cast<char*>(tempRow.data()),_heightRawSize.x * sizeof(uint16_t)))
        {
            std::wcerr << L"Failed to read raw data from file: " << rawData << std::endl;
            return;
        }

        for(int x = 0; x < _heightRawSize.x; ++x) {
            _heightRawMapData[z][x] = static_cast<float>(tempRow[x]) / 65535.0f * _fieldSize.y;
        }
    }

    file.close();
}

bool Terrain::InBounds(int32 z, int32 x) const
{
    return
        z >= 0 && z < static_cast<int32>(_heightRawSize.y) &&
        x >= 0 && x < static_cast<int32>(_heightRawSize.x);
}

void Terrain::Smooth()
{
    vector<vector<float>> temp;

	temp = _heightRawMapData;

    for(uint32 i = 0; i < _heightRawSize.y; ++i)
    {
        for(uint32 j = 0; j <_heightRawSize.x; ++j)
        {
            temp[i][j] = Average(i,j);
        }
    }

    _heightRawMapData = temp;
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
                avg += _heightRawMapData[z][x];
                num += 1.0f;
            }
        }
    }

    return avg / num;
}
