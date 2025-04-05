#include "pch.h"
#include "PathStamp.h"
#include "Gizmo.h"
#include "CameraManager.h"
#include "Camera.h"

std::unique_ptr<PathStamp> PathStamp::main = nullptr;


void PathStamp::Init()
{
}

void PathStamp::Run()
{

	if (Input::main->GetKeyDown(KeyCode::Enter))
	{
		vec3 cameraPos = CameraManager::main->GetActiveCamera()->GetCameraPos();
		_controllPoint.push_back(cameraPos);
	}

	const int segmentCount = 30;
	Gizmo::main->Width(3.0f);

	_lineData.clear();

	for (int i = 0; i + 2 < _controllPoint.size(); i += 2)
	{
		vec3 p0 = _controllPoint[i];
		vec3 p1 = _controllPoint[i + 1];
		vec3 p2 = _controllPoint[i + 2];

		for (int j = 0; j < segmentCount; ++j)
		{
			float t1 = (float)j / segmentCount;
			float t2 = (float)(j + 1) / segmentCount;

			vec3 a = BezierQuadratic(p0, p1, p2, t1);
			vec3 b = BezierQuadratic(p0, p1, p2, t2);

			_lineData.push_back(a);
			_lineData.push_back(b);

			Gizmo::main->Line(a, b, vec4(1, 0, 0, 1));
		}
	}

	if (Input::main->GetKeyDown(KeyCode::End))
	{
		if (_lineData.size() >= 2)
		{
			vec3 first = _lineData.front();
			vec3 last = _lineData.back();

			_lineData.push_back(last);
			_lineData.push_back(first);
		}
	}


	if (Input::main->GetKeyDown(KeyCode::Delete))
	{
		if (!_controllPoint.empty())
		{
			_controllPoint.pop_back();
		}
	}

	if (Input::main->GetKeyDown(KeyCode::Insert))
	{
		std::ofstream file("BezierLines.txt");

		if (!file.is_open())
		{
			std::cerr << "베지어 파일 저장 실패" << std::endl;
			return;
		}

		for (const auto& point : _lineData)
		{
			file << point.x << " " << point.y << " " << point.z << std::endl;
		}

		file.close();
	}






}

vec3 PathStamp::BezierQuadratic(vec3& p0, vec3& p1, vec3& p2, float& t)
{
	return (1 - t) * (1 - t) * p0 +
		2 * (1 - t) * t * p1 +
		t * t * p2;
}

vec3 PathStamp::BezierCubic(const vec3& p0, const vec3& p1, const vec3& p2, const vec3& p3, float& t)
{
	float u = 1 - t;
	return u * u * u * p0 +
		3 * u * u * t * p1 +
		3 * u * t * t * p2 +
		t * t * t * p3;
}


