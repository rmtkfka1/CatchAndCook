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
		lines.push_back(cameraPos);
	}


	for (int i = 0; i < lines.size(); ++i)
	{
		//1개 
		//i ==0 => lines[0] lines[0] 

		//2개
		//i == 0 => lines[0] lines[1]
		//i == 1 => lines[1] lines[0]

		//3개
		//i == 0 => lines[0] lines[1]
		//i == 1 => lines[1] lines[2]
		//i == 2 => lines[2] lines[0]

		//4개
		//i == 0 => lines[0] lines[1]
		//i == 1 => lines[1] lines[2]
		//i == 2 => lines[2] lines[3]
		//i == 3 => lines[3] lines[0]

		vec3 from = lines[i];
		vec3 to = lines[(i + 1) % lines.size()];
		Gizmo::main->Width(3.0f);
		Gizmo::main->Line(from, to, vec4(1, 0, 0, 1));
	}

}

