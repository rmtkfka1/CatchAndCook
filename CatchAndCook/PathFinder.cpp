#include "pch.h"
#include "PathFinder.h"
#include "Transform.h"
#include "Gizmo.h"
#include "simple_mesh_ext.h"

unordered_map<wstring, FishPath> PathFinder::_pathList;

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
    _firstQuat = GetOwner()->_transform->GetWorldRotation();
}

void PathFinder::Update()
{
    if (_pathList[_pathName].path.size() < 2 && _pathName ==L"Null")
		assert(false);

	const vector<vec3>& myPath = _pathList[_pathName].path;

    vec3 start = myPath[_currentIndex];
    vec3 end = myPath[(_currentIndex + 1) % myPath.size()];

    if (_segmentLength == 0.0f)
        _segmentLength = (end - start).Length();

    _distanceMoved += Time::main->GetDeltaTime() * _moveSpeed;

    float t = std::clamp(_distanceMoved / _segmentLength, 0.0f, 1.0f);
    vec3 pos = vec3::Lerp(start, end, t);
    vec3 currentPos = GetOwner()->_transform->SetWorldPosition(pos);

    vec3 dir = end - start;

    if (dir.LengthSquared() > 0.0001f)
    {
        dir.Normalize();
        GetOwner()->_transform->LookUpSmooth(dir, vec3::Up,3.0f,_firstQuat);
    }

    if (t >= 1.0f)
    {
        _currentIndex = (_currentIndex + 1) % myPath.size();
        _distanceMoved = 0.0f;
        _segmentLength = 0.0f;
    }


	if (_pathList[_pathName].AreyouDraw==false)
	{
        for (size_t i = 0; i < myPath.size() - 1; ++i)
        {
            Gizmo::main->Line(myPath[i], myPath[i + 1], vec4(1, 1, 0, 1));
        }

        if (myPath.size() >= 2)
        {
            Gizmo::main->Line(myPath.back(), myPath.front(), vec4(1, 1, 0, 1));
        }

		_pathList[_pathName].AreyouDraw = true;
	}

};
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
        vec3 point(x, y, z);
        _pathList[fileName].path.push_back(point);
    }

    file.close();
     cout << "라인 데이터: " << _pathList[fileName].path.size() << "개 읽음." << std::endl;
}

void PathFinder::ClearDebugDraw()
{
	for (auto& path : _pathList)
	{
		path.second.AreyouDraw = false;
	}
}

void PathFinder::SetPass(const wstring& path)
{
    if (_pathList.find(path) == _pathList.end())
    {
        ReadPathFile(path);
    }

    _pathName = path;
}
