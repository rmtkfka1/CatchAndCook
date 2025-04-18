#include "pch.h"
#include "FishMonster.h"
#include "Transform.h"
#include "Gizmo.h"
#include "AnimationListComponent.h"
#include "SkinnedHierarchy.h"

FishMonster::FishMonster()
{
}

FishMonster::~FishMonster()
{
}

void FishMonster::Init()
{
}

void FishMonster::Start()
{
	_firstQuat = GetOwner()->_transform->GetWorldRotation();

	std::unordered_map<string, std::shared_ptr<Animation>> aniList;



}

void FishMonster::Update()
{
	float dt = Time::main->GetDeltaTime();

	UpdateState(dt);
}

void FishMonster::Update2()
{
}

void FishMonster::Enable()
{
}

void FishMonster::Disable()
{
}

void FishMonster::RenderBegin()
{
}

void FishMonster::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FishMonster::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void FishMonster::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void FishMonster::SetDestroy()
{
}

void FishMonster::Destroy()
{

}


void FishMonster::UpdateState(float dt)
{
	switch (_state)
	{
	case FishMonsterState::Idle:
		Idle(dt);
		break;
	case FishMonsterState::Move:
		Move(dt);
		break;
	case FishMonsterState::Attack:
		Attack(dt);
		break;
	case FishMonsterState::Die:
		Die(dt);
		break;
	case FishMonsterState::Hit:
		Hit(dt);
		break;
	default:
		break;
	}
}

void FishMonster::SetState(FishMonsterState state)
{
	if (_state == state)
		return;
	_state = state;

	switch (_state)
	{
	case FishMonsterState::Idle:
		break;
	case FishMonsterState::Move:
		break;
	case FishMonsterState::Attack:
		break;
	case FishMonsterState::Die:
		break;
	case FishMonsterState::Hit:
		break;
	default:
		break;
	}
}

void FishMonster::Idle(float dt)
{

}

void FishMonster::Move(float dt)
{

	//if (_fishPath.size() < 2 && _pathName == L"Null")
	//	assert(false);

	const vector<vec3>& myPath = _fishPath;

	int nextIndex = _forward ? _currentIndex + 1 : _currentIndex - 1;

	const vec3& start = myPath[_currentIndex];
	const vec3& end = myPath[nextIndex];

	if (_segmentLength < 0.0001f)
		_segmentLength = (end - start).Length();

	_distanceMoved += Time::main->GetDeltaTime() * _moveSpeed;

	float t = std::clamp(_distanceMoved / _segmentLength, 0.0f, 1.0f);
	vec3 pos = vec3::Lerp(start, end, t);
	vec3 currentPos = GetOwner()->_transform->SetWorldPosition(pos);

	vec3 dir = end - start;

	if (dir.LengthSquared() > 0.0001f)
	{
		dir.Normalize();
		GetOwner()->_transform->LookUpSmooth(dir, vec3::Up, 6.0f, _firstQuat);
	}

	if (t >= 1.0f)
	{
		_distanceMoved = 0.0f;
		_segmentLength = 0.0f;

		if (_forward)
		{
			_currentIndex += 1;

			if (_currentIndex >= myPath.size()-1)
			{
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

	for (size_t i = 0; i < myPath.size() - 1; ++i)
	{
		Gizmo::main->Line(myPath[i], myPath[i + 1], vec4(1, 1, 0, 1));
	}

	//cout << "Current Index: " << _currentIndex << ", Next: " << nextIndex << endl;
	//cout << "Forward: " << _forward << ", t: " << t << ", DistanceMoved: " << _distanceMoved << endl;



}

void FishMonster::Attack(float dt)
{
}

void FishMonster::Die(float dt)
{
}

void FishMonster::Hit(float dt)
{
}

void FishMonster::ReadPathFile(const std::wstring& fileName)
{
	const wstring path = L"../Resources/Graph/";;

	_pathName = fileName;

	std::ifstream file(path+fileName);

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
		_fishPath.push_back(point);
	}

	file.close();
	cout << "라인 데이터: " << _fishPath.size() << "개 읽음." << std::endl;
}