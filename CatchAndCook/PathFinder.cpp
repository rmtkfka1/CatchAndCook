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
	_pathOffset = GenerateRandomPointInSphere(500.0f);
}

void PathFinder::Start()
{
    _firstQuat = GetOwner()->_transform->GetWorldRotation();

	if (auto renderer = GetOwner()->GetRenderer())
	{
		renderer->AddStructuredSetter(static_pointer_cast<PathFinder>(shared_from_this()), BufferType::SeaFIshParam);
		_renderBase = renderer;
	}
}

void PathFinder::Update()
{

	const vector<vec3>& myPath = _pathList[_pathName].path;

	int nextIndex = _forward ? _currentIndex + 1 : _currentIndex - 1;

	const vec3& start = myPath[_currentIndex];
	const vec3& end = myPath[nextIndex];

	if (_segmentLength < 0.0001f)
		_segmentLength = (end - start).Length();

	_distanceMoved += Time::main->GetDeltaTime() * _moveSpeed;

	float t = std::clamp(_distanceMoved / _segmentLength, 0.0f, 1.0f);
	vec3 pos = vec3::Lerp(start, end, t);
	vec3 finalPos = pos + _pathOffset;

	vec3 currentPos = GetOwner()->_transform->SetWorldPosition(finalPos);

	vec3 dir = end - start;

	if (dir.LengthSquared() > 0.0001f)
	{
		dir.Normalize();
		GetOwner()->_transform->LookUpSmooth(dir, vec3::Up, 3.0f, _firstQuat);
	}

	if (t >= 1.0f)
	{
		_distanceMoved = 0.0f;
		_segmentLength = 0.0f;

		if (_forward)
		{
			_currentIndex += 1;

			if (_currentIndex >= myPath.size() - 1)
			{
				cout << "forward false " << endl;
				_forward = false;
				return;
			}
		}

		else
		{
			_currentIndex -= 1;

			if (_currentIndex <= 0)
			{
				_forward = true;
				return;
			}

		}
	}


	if (_pathList[_pathName].AreyouDraw == false)
	{
		for (size_t i = 0; i < myPath.size() - 1; ++i)
		{
			Gizmo::main->Line(myPath[i], myPath[i + 1], vec4(1, 1, 0, 1));
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
	const wstring path = L"../Resources/Graph/";;

    std::ifstream file(path + fileName);
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

vec3 PathFinder::GenerateRandomPointInSphere(float radius)
{
	float u = Range(0.0f, 1.0f);
	float theta = Range(0.0f, 2.0f *3.14f);
	float phi = acos(2.0f * u - 1.0f);
	float r = radius * cbrt(Range(0.0f, 1.0f));

	float x = r * sin(phi) * cos(theta);
	float y = r * sin(phi) * sin(theta);
	float z = r * cos(phi);

	return vec3(x, y, z);
}

float PathFinder::Range(float min, float max)
{
	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (max - min);
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
		cout << "read" << endl;
        ReadPathFile(path);
    }

    _pathName = path;
}

void PathFinder::SetData(StructuredBuffer* buffer, Material* material)
{

	FishInfo info;
	info.fishSpeed = 4.0f;
	info.fishWaveAmount = 0.5f;

	BoundingBox& box = _renderBase.lock()->GetOriginBound();
	info.boundsSizeZ = box.Extents.z;
	info.boundsCenterZ = box.Center.z;

	buffer->AddData(info);
}
