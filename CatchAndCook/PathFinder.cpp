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
    if (_paths.size() < 2 || _controllPoints.size() < 2)
        return;

    vec3 start = _paths[currentIndex];
    vec3 end = _paths[(currentIndex + 1) % _paths.size()];

    if (segmentLength == 0.0f)
        segmentLength = (end - start).Length();

    distanceMoved += Time::main->GetDeltaTime() * moveSpeed;

    float t = std::clamp(distanceMoved / segmentLength, 0.0f, 1.0f);
    vec3 pos = vec3::Lerp(start, end, t);
    vec3 currentPos = GetOwner()->_transform->SetLocalPosition(pos);



    if (t >= 1.0f)
    {
        currentIndex = (currentIndex + 1) % _paths.size();
        distanceMoved = 0.0f;
        segmentLength = 0.0f;
    }

    // 시각화
    for (size_t i = 0; i < _paths.size() - 1; ++i)
    {
        Gizmo::main->Line(_paths[i], _paths[i + 1], vec4(1, 1, 0, 1));
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

    _controllPoints.clear();
    _paths.clear();

    std::string line;
    enum class Section { None, ControlPoints, LineData } section = Section::None;

    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        if (line == "# ControlPoints")
        {
            section = Section::ControlPoints;
            continue;
        }
        if (line == "# LineData")
        {
            section = Section::LineData;
            continue;
        }

        std::istringstream ss(line);
        float x, y, z;
        ss >> x >> y >> z;
        vec3 point(x, y, z);

        if (section == Section::ControlPoints)
            _controllPoints.push_back(point);
        else if (section == Section::LineData)
            _paths.push_back(point);
    }

    file.close();

    std::cout << "컨트롤 포인트: " << _controllPoints.size() << "개, "
        << "라인 데이터: " << _paths.size() << "개 읽음." << std::endl;
}