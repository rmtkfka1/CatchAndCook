#include "pch.h"
#include "PathFinder.h"
#include "Transform.h"
#include "Gizmo.h"
PathFinder::PathFinder()
{
}

PathFinder::~PathFinder()
{
}

void PathFinder::Init()
{
}

void PathFinder::Start()
{
}

void PathFinder::Update()
{

    if (_paths.size() < 2) return;

    vec3 start = _paths[currentIndex];
    vec3 end = _paths[(currentIndex + 1) % _paths.size()];

    if (segmentLength == 0.0f)
        segmentLength = (end - start).Length();

    distanceMoved += Time::main->GetDeltaTime() * moveSpeed;

    float t = std::clamp(distanceMoved / segmentLength, 0.0f, 1.0f);
    vec3 pos = vec3::Lerp(start, end, t);
    GetOwner()->_transform->SetLocalPosition(pos);

    if (t >= 1.0f)
    {
        currentIndex = (currentIndex + 1) % _paths.size();
        distanceMoved = 0.0f;
        segmentLength = 0.0f;
    }

    for (size_t i = 0; i < _paths.size() - 1; ++i)
    {
        vec3 from = _paths[i];
        vec3 to = _paths[i + 1];
        Gizmo::main->Line(from, to, vec4(1, 1, 0, 1)); 
    }

    if (_paths.size() >= 2)
    {
        Gizmo::main->Line(_paths.back(), _paths.front(), vec4(1, 1, 0, 1));
    }

}

void PathFinder::Update2()
{
}

void PathFinder::RenderBegin()
{
}

void PathFinder::Enable()
{
}

void PathFinder::Disable()
{
}

void PathFinder::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PathFinder::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void PathFinder::ReadPathFile(const std::wstring& fileName)
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        std::wcout << L"Failed to open file: " << fileName << std::endl;
        return;
    }

    std::string line;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        std::istringstream ss(line);
        float x, y, z;
        ss >> x >> y >> z;

        _paths.emplace_back(vec3(x, y, z));
    }

    file.close();
    std::cout << "Read " << _paths.size() << " points from file." << std::endl;

}
