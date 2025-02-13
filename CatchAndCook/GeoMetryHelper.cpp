#include "pch.h"
#include "GeoMetryHelper.h"
#include "Mesh.h"

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBox(const float scale)
{

    // 리소스 키 생성
    std::wstring key = L"box" + std::to_wstring(scale);

    // 이미 로드된 Mesh가 있다면 반환
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(key);
    if (mesh)
    {
        return mesh;
    }

    // Vertex 및 Index 데이터 생성
    vector<Vertex_GeoMetry> vertices;
    vertices.reserve(24); // 6면 x 4개 정점

    vector<uint32> indices;
    indices.reserve(36); // 6면 x 2개의 삼각형 x 3개의 인덱스


    float w2 = scale;
    float h2 = scale;
    float d2 = scale;

    vector<Vertex_GeoMetry> vec(24);

    // 앞면
    vec[0] = Vertex_GeoMetry(vec3(-w2, -h2, -d2), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[1] = Vertex_GeoMetry(vec3(-w2, +h2, -d2), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[2] = Vertex_GeoMetry(vec3(+w2, +h2, -d2), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[3] = Vertex_GeoMetry(vec3(+w2, -h2, -d2), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    // 뒷면
    vec[4] = Vertex_GeoMetry(vec3(-w2, -h2, +d2), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    vec[5] = Vertex_GeoMetry(vec3(+w2, -h2, +d2), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    vec[6] = Vertex_GeoMetry(vec3(+w2, +h2, +d2), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
    vec[7] = Vertex_GeoMetry(vec3(-w2, +h2, +d2), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));
    // 윗면
    vec[8] = Vertex_GeoMetry(vec3(-w2, +h2, -d2), vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
    vec[9] = Vertex_GeoMetry(vec3(-w2, +h2, +d2), vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec[10] = Vertex_GeoMetry(vec3(+w2, +h2, +d2), vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    vec[11] = Vertex_GeoMetry(vec3(+w2, +h2, -d2), vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f));
    // 아랫면
    vec[12] = Vertex_GeoMetry(vec3(-w2, -h2, -d2), vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f));
    vec[13] = Vertex_GeoMetry(vec3(+w2, -h2, -d2), vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f));
    vec[14] = Vertex_GeoMetry(vec3(+w2, -h2, +d2), vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    vec[15] = Vertex_GeoMetry(vec3(-w2, -h2, +d2), vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    // 왼쪽면
    vec[16] = Vertex_GeoMetry(vec3(-w2, -h2, +d2), vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f));
    vec[17] = Vertex_GeoMetry(vec3(-w2, +h2, +d2), vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
    vec[18] = Vertex_GeoMetry(vec3(-w2, +h2, -d2), vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f));
    vec[19] = Vertex_GeoMetry(vec3(-w2, -h2, -d2), vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f));
    // 오른쪽면
    vec[20] = Vertex_GeoMetry(vec3(+w2, -h2, -d2), vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[21] = Vertex_GeoMetry(vec3(+w2, +h2, -d2), vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[22] = Vertex_GeoMetry(vec3(+w2, +h2, +d2), vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    vec[23] = Vertex_GeoMetry(vec3(+w2, -h2, +d2), vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f));

    vector<uint32> idx(36);

    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;
    // 뒷면
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;
    // 윗면
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;
    // 아랫면
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;
    // 왼쪽면
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;
    // 오른쪽면
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;


    // Mesh 초기화 및 리소스 등록
    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);
    ResourceManager::main->Add(key, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBoxWithColor(const float scale, vec4 color)
{
    // 리소스 키 생성
    //std::wstring key = L"boxColor" + std::to_wstring(scale);

    //// 이미 로드된 Mesh가 있다면 반환
    shared_ptr<Mesh> mesh;
    //if (mesh)
    //{
    //    return mesh;
    //}

    // Vertex 및 Index 데이터 생성
    vector<Vertex_Color> vertices;
    vertices.reserve(24); // 6면 x 4개 정점

    vector<uint32> indices;
    indices.reserve(36); // 6면 x 2개의 삼각형 x 3개의 인덱스


    float w2 = scale;
    float h2 = scale;
    float d2 = scale;

    vector<Vertex_Color> vec(24);

    // 앞면
    vec[0] = Vertex_Color(vec3(-w2, -h2, -d2), vec2(0.0f, 1.0f), color);
    vec[1] = Vertex_Color(vec3(-w2, +h2, -d2), vec2(0.0f, 0.0f), color);
    vec[2] = Vertex_Color(vec3(+w2, +h2, -d2), vec2(1.0f, 0.0f), color);
    vec[3] = Vertex_Color(vec3(+w2, -h2, -d2), vec2(1.0f, 1.0f), color);
    // 뒷면
    vec[4] = Vertex_Color(vec3(-w2, -h2, +d2), vec2(1.0f, 1.0f), color);
    vec[5] = Vertex_Color(vec3(+w2, -h2, +d2), vec2(0.0f, 1.0f), color);
    vec[6] = Vertex_Color(vec3(+w2, +h2, +d2), vec2(0.0f, 0.0f), color);
    vec[7] = Vertex_Color(vec3(-w2, +h2, +d2), vec2(1.0f, 0.0f), color);
    // 윗면
    vec[8] = Vertex_Color(vec3(-w2, +h2, -d2), vec2(0.0f, 1.0f), color);
    vec[9] = Vertex_Color(vec3(-w2, +h2, +d2), vec2(0.0f, 0.0f), color);
    vec[10] = Vertex_Color(vec3(+w2, +h2, +d2), vec2(1.0f, 0.0f),color);
    vec[11] = Vertex_Color(vec3(+w2, +h2, -d2), vec2(1.0f, 1.0f),color);
    // 아랫면
    vec[12] = Vertex_Color(vec3(-w2, -h2, -d2), vec2(1.0f, 1.0f),color);
    vec[13] = Vertex_Color(vec3(+w2, -h2, -d2), vec2(0.0f, 1.0f),color);
    vec[14] = Vertex_Color(vec3(+w2, -h2, +d2), vec2(0.0f, 0.0f),color);
    vec[15] = Vertex_Color(vec3(-w2, -h2, +d2), vec2(1.0f, 0.0f),color);
    // 왼쪽면
    vec[16] = Vertex_Color(vec3(-w2, -h2, +d2), vec2(0.0f, 1.0f), color);
    vec[17] = Vertex_Color(vec3(-w2, +h2, +d2), vec2(0.0f, 0.0f), color);
    vec[18] = Vertex_Color(vec3(-w2, +h2, -d2), vec2(1.0f, 0.0f), color);
    vec[19] = Vertex_Color(vec3(-w2, -h2, -d2), vec2(1.0f, 1.0f), color);
    // 오른쪽면
    vec[20] = Vertex_Color(vec3(+w2, -h2, -d2), vec2(0.0f, 1.0f),color);
    vec[21] = Vertex_Color(vec3(+w2, +h2, -d2), vec2(0.0f, 0.0f),color);
    vec[22] = Vertex_Color(vec3(+w2, +h2, +d2), vec2(1.0f, 0.0f),color);
    vec[23] = Vertex_Color(vec3(+w2, -h2, +d2), vec2(1.0f, 1.0f),color);

    vector<uint32> idx(36);

    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;
    // 뒷면
    idx[6] = 4; idx[7] = 5; idx[8] = 6;
    idx[9] = 4; idx[10] = 6; idx[11] = 7;
    // 윗면
    idx[12] = 8; idx[13] = 9; idx[14] = 10;
    idx[15] = 8; idx[16] = 10; idx[17] = 11;
    // 아랫면
    idx[18] = 12; idx[19] = 13; idx[20] = 14;
    idx[21] = 12; idx[22] = 14; idx[23] = 15;
    // 왼쪽면
    idx[24] = 16; idx[25] = 17; idx[26] = 18;
    idx[27] = 16; idx[28] = 18; idx[29] = 19;
    // 오른쪽면
    idx[30] = 20; idx[31] = 21; idx[32] = 22;
    idx[33] = 20; idx[34] = 22; idx[35] = 23;


    // Mesh 초기화 및 리소스 등록
    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleMesh(const float scale)
{

    std::wstringstream wss;
    wss << scale;
    wstring temp = wstring(wss.str());

    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"RECTANGLE" + temp);

    if (mesh)
    {
        return mesh;
    }

    vector<Vertex_GeoMetry> vec(4);

    // 앞면
    vec[0] = Vertex_GeoMetry(vec3(-scale, -scale, 0), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[1] = Vertex_GeoMetry(vec3(-scale, +scale, 0), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[2] = Vertex_GeoMetry(vec3(+scale, +scale, 0), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[3] = Vertex_GeoMetry(vec3(+scale, -scale, 0), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));

    vector<uint32> idx(6);

    // 앞면
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"RECTANGLE" + temp, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadGripMeshControlPoints(float width, float height, int division_x, int division_z)
{

    shared_ptr<Mesh> mesh = make_shared<Mesh>();

    const float dx = width / division_x;
    const float dz = height / division_z;

    vector<Vertex_GeoMetry> vertices;

    vertices.reserve((division_x + 1) * (division_z + 1));


    //leftBottom
    vec3 leftBottom = vec3(-0.5f * width, 0, -0.5f * height);

    for (int j = 0; j <= division_z; ++j)
    {
        vec3 startPoint = leftBottom + vec3(0, 0, dz * j);

        //아랫줄
        for (int i = 0; i <= division_x; ++i)
        {
            Vertex_GeoMetry v;
            //xz 평면에서 x 방향으로이동.
            v.position = startPoint;
            v.position.x += dx * i;
            v.normal = vec3(0, 1, 0);
            v.uv = vec2(float(i) / division_x, 1.0f - float(j) / division_z);
            vertices.push_back(v);
        }
    }


    vector<uint32> indices;

    for (int j = 0; j < division_z; ++j)
    {
        const int offset = (division_x + 1) * j;

        for (int i = 0; i < division_x; ++i)
        {
            // 4개의 정점으로 이루어진 패치를 구성
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1);
            indices.push_back(offset + i + division_x + 1);
            indices.push_back(offset + i + 1 + division_x + 1);
        }
    }


    mesh->Init(vertices, indices);

    return mesh;

}

shared_ptr<Mesh> GeoMetryHelper::LoadGripMesh(float width, float height, int division_x, int division_z)
{
    shared_ptr<Mesh> mesh = make_shared<Mesh>();

    const float dx = width / division_x;
    const float dz = height / division_z;

    vector<Vertex_GeoMetry> vertices;

    vertices.reserve((division_x + 1) * (division_z + 1));


    //leftBottom
    vec3 leftBottom = vec3(-0.5f * width, 0, -0.5f * height);

    for (int j = 0; j <= division_z; ++j)
    {
        vec3 startPoint = leftBottom + vec3(0, 0, dz * j);

        //아랫줄
        for (int i = 0; i <= division_x; ++i)
        {
            Vertex_GeoMetry v;
            //xz 평면에서 x 방향으로이동.
            v.position = startPoint;
            v.position.x += dx * i;
            v.normal = vec3(0, 1, 0);
            v.uv = vec2(float(i) / division_x, 1.0f - float(j) / division_z);
            vertices.push_back(v);
        }
    }


    vector<uint32> indices;

    for (int j = 0; j < division_z; ++j)
    {
        const int offset = (division_x + 1) * j;

        for (int i = 0; i < division_x; ++i)
        {
            //윗쪽
            indices.push_back(offset + i);
            indices.push_back(offset + i + division_x + 1);
            indices.push_back(offset + (i + 1) + division_x + 1);

            //아랫쪽
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + division_x + 1);
            indices.push_back(offset + i + 1);
        }
    }

	for(auto& vertice : vertices)
	{
        Matrix mat = Matrix::CreateTranslation(width/2,0,height/2);
        vertice.position = vec3::Transform(vertice.position,mat);
	}

    mesh->Init(vertices, indices);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadSprtieMesh()
{
    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"sprtieMesh");

    if (mesh)
    {
        return mesh;
    }

    vector<Vertex_Color> vec(4);

    // 앞면
    vec[0] = Vertex_Color(vec3(0.0f, 1.0f, 0), vec2(0.0f, 1.0f));
    vec[1] = Vertex_Color(vec3(0.0f, 0.0f, 0), vec2(0.0f, 0.0f));
    vec[2] = Vertex_Color(vec3(1.0f, 0.0f, 0), vec2(1.0f, 0.0f));
    vec[3] = Vertex_Color(vec3(1.0f, 1.0f, 0), vec2(1.0f, 1.0f));

    vector<uint32> idx = { 0,1,2,0,2,3 };

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"sprtieMesh", mesh);

    return mesh;

}
