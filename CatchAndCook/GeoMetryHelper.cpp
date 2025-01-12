#include "pch.h"
#include "GeoMetryHelper.h"
#include "Mesh.h"

shared_ptr<Mesh> GeoMetryHelper::LoadRectangleBox(const float scale)
{

    std::wstringstream wss;
    wss << scale;
    wstring temp = wstring(wss.str());

    shared_ptr<Mesh> mesh = ResourceManager::main->Get<Mesh>(L"box" + temp);

    if (mesh)
    {
        return mesh;
    }

    vector<Vertex_Static> v;
    v.reserve(24);

    // À­¸é
    v.push_back({ vec3(-1.0f, 1.0f, -1.0f) * scale, vec2(0.0f, 0.0f),vec3(0.0f, 1.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, -1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f) });

    // ¾Æ·§¸é
    v.push_back({ vec3(-1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, -1.0f, -1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f) });
    v.push_back({ vec3(1.0f, -1.0f, 1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f) });

    // ¾Õ¸é
    v.push_back({ vec3(-1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f) });
    v.push_back({ vec3(1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f) });
    v.push_back({ vec3(1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f) });

    // µÞ¸é
    v.push_back({ vec3(-1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) });
    v.push_back({ vec3(1.0f, -1.0f, 1.0f) * scale, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f) });
    v.push_back({ vec3(1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, 1.0f) * scale, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f) });

    // ¿ÞÂÊ
    v.push_back({ vec3(-1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, 1.0f) * scale, vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(-1.0f, -1.0f, -1.0f) * scale, vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f) });

    // ¿À¸¥ÂÊ
    v.push_back({ vec3(1.0f, -1.0f, 1.0f) * scale, vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(1.0f, -1.0f, -1.0f) * scale, vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, -1.0f) * scale, vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) });
    v.push_back({ vec3(1.0f, 1.0f, 1.0f) * scale, vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f) });

    // ÀÎµ¦½º ¹öÆÛ
    vector<uint32> index = {
        0,  1,  2,  0,  2,  3,  // À­¸é
        4,  5,  6,  4,  6,  7,  // ¾Æ·§¸é
        8,  9,  10, 8,  10, 11, // ¾Õ¸é
        12, 13, 14, 12, 14, 15, // µÞ¸é
        16, 17, 18, 16, 18, 19, // ¿ÞÂÊ
        20, 21, 22, 20, 22, 23  // ¿À¸¥ÂÊ
    };


    mesh = make_shared<Mesh>();
    mesh->Init(v, index);

    ResourceManager::main->Add(L"BOX" + temp, mesh);

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

    vector<Vertex_Static> vec(4);

    // ¾Õ¸é
    vec[0] = Vertex_Static(vec3(-scale, -scale, 0), vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[1] = Vertex_Static(vec3(-scale, +scale, 0), vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[2] = Vertex_Static(vec3(+scale, +scale, 0), vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f));
    vec[3] = Vertex_Static(vec3(+scale, -scale, 0), vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f));

    vector<uint32> idx(6);

    // ¾Õ¸é
    idx[0] = 0; idx[1] = 1; idx[2] = 2;
    idx[3] = 0; idx[4] = 2; idx[5] = 3;

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"RECTANGLE" + temp, mesh);

    return mesh;
}

shared_ptr<Mesh> GeoMetryHelper::LoadGripMesh(float width, float height, int division_x, int division_z)
{

    shared_ptr<Mesh> mesh = make_shared<Mesh>();

    const float dx = width / division_x;
    const float dz = height / division_z;

    vector<Vertex_Static> vertices;

    vertices.reserve((division_x + 1) * (division_z + 1));


    //leftBottom
    vec3 leftBottom = vec3(-0.5f * width, 0, -0.5f * height);

    for (int j = 0; j <= division_z; ++j)
    {
        vec3 startPoint = leftBottom + vec3(0, 0, dz * j);

        //¾Æ·§ÁÙ
        for (int i = 0; i <= division_x; ++i)
        {
            Vertex_Static v;
            //xz Æò¸é¿¡¼­ x ¹æÇâÀ¸·ÎÀÌµ¿.
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
            //À­ÂÊ
            indices.push_back(offset + i);
            indices.push_back(offset + i + division_x + 1);
            indices.push_back(offset + (i + 1) + division_x + 1);

            //¾Æ·§ÂÊ
            indices.push_back(offset + i);
            indices.push_back(offset + i + 1 + division_x + 1);
            indices.push_back(offset + i + 1);
        }
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

    vector<Vertex_Sprite> vec(4);

    // ¾Õ¸é
    vec[0] = Vertex_Sprite(vec3(0.0f, 1.0f, 0), vec2(0.0f, 1.0f));
    vec[1] = Vertex_Sprite(vec3(0.0f, 0.0f, 0), vec2(0.0f, 0.0f));
    vec[2] = Vertex_Sprite(vec3(1.0f, 0.0f, 0), vec2(1.0f, 0.0f));
    vec[3] = Vertex_Sprite(vec3(1.0f, 1.0f, 0), vec2(1.0f, 1.0f));

    vector<uint32> idx = { 0,1,2,0,2,3 };

    mesh = make_shared<Mesh>();
    mesh->Init(vec, idx);

    ResourceManager::main->Add(L"sprtieMesh", mesh);

    return mesh;

}
