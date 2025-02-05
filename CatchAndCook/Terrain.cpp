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

    // 좌표를 지형의 로컬 좌표계로 변환
    float tempX = x + _gridSize.x / 2;
    float tempZ = z + _gridSize.y / 2;

    tempX -= terrainOrigin.x;
    tempZ -= terrainOrigin.z;

    // 경계 조건 확인
    if(tempX < 0 || tempX >= _gridSize.x || tempZ < 0 || tempZ >= _gridSize.y)
    {
        return 0; // 경계를 벗어나면 높이 0 반환
    }

    // 그리드 셀 좌표 계산
    float ratioX = tempX / _gridSize.x;
    float ratioZ = tempZ / _gridSize.y;

    float exactX = ratioX * (_size.x - 1);
    float exactZ = ratioZ * (_size.y - 1);

    // 현재 좌표가 속한 그리드 셀의 행과 열
    int ix0 = static_cast<int>(exactX);
    int iz0 = static_cast<int>(exactZ);
    int ix1 = ix0 + 1;
    int iz1 = iz0 + 1;

    // 경계 조건 처리
    ix1 = ix1 >= _size.x ? ix0 : ix1;
    iz1 = iz1 >= _size.y ? iz0 : iz1;

    // 현재 셀의 네 꼭짓점 높이 값
    float A = _rawData[iz0][ix0]; // 왼쪽 아래
    float B = _rawData[iz0][ix1]; // 오른쪽 아래
    float C = _rawData[iz1][ix0]; // 왼쪽 위
    float D = _rawData[iz1][ix1]; // 오른쪽 위

    // 현재 좌표가 속한 삼각형 결정
    float s = exactX - ix0; // x 방향 보간 비율
    float t = exactZ - iz0; // z 방향 보간 비율

    // 상단 삼각형 (ABC) 또는 하단 삼각형 (DCB) 결정
    if(s + t <= 1.0f) // 상단 삼각형 (ABC)
    {
        float uy = B - A; // B와 A의 높이 차이
        float vy = C - A; // C와 A의 높이 차이
        return terrainOrigin.y + A + s * uy + t * vy+ 6.0f ; // 보간 결과 반환
    } 

    else // 하단 삼각형 (DCB)
    {
        float uy = C - D; // C와 D의 높이 차이
        float vy = B - D; // B와 D의 높이 차이
        return terrainOrigin.y + D + (1.0f - s) * uy + (1.0f - t) * vy + 6.0f; // 보간 결과 반환
    }
}