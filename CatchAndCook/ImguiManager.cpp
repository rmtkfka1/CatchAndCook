#include "pch.h"
#include "ImguiManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "LightManager.h"
#include "WaterController.h"
#include <commdlg.h>
#include "ComputeManager.h"
#include "Gizmo.h"

unique_ptr<ImguiManager> ImguiManager::main;

ImguiManager::~ImguiManager()
{
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImguiManager::Init()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(Core::main->GetHandle());
	ImGui_ImplDX12_Init(Core::main->GetDevice().Get(), 2,
		DXGI_FORMAT_R8G8B8A8_UNORM, Core::main->GetImguiHeap().Get(),
		Core::main->GetImguiHeap()->GetCPUDescriptorHandleForHeapStart(),
		Core::main->GetImguiHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImguiManager::Render()
{
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	Debug();
	Compute();
	Lighting();
	SeaMove();
	SeaColor();


	ImGui::SliderFloat("separationWeight", separationWeight, 0, 100.0f);
	ImGui::SliderFloat("alignmentWeight", alignmentWeight, 0, 100.0f);
	ImGui::SliderFloat("cohesionWeight", cohesionWeight, 0, 100.0f);

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Core::main->GetCmdList().Get());
}

void ImguiManager::SeaMove()
{
	ImGui::Begin("sea_move");

	if (_seaParam)
	{
		ImGui::SliderInt("Wave Count", &_seaParam->wave_count, 0, 10);

		for (int i = 0; i < _seaParam->wave_count; i++)
		{
			char label[64];

			sprintf_s(label, "Wave %d Amplitude", i);
			ImGui::SliderFloat(label, &_seaParam->waves[i].amplitude, 0.0f, 10.0f);

			sprintf_s(label, "Wave %d Wavelength", i);
			ImGui::SliderFloat(label, &_seaParam->waves[i].wavelength, 0.0f, 2000.0f);

			sprintf_s(label, "Wave %d Speed", i);
			ImGui::SliderFloat(label, &_seaParam->waves[i].speed, 0.0f, 10.0f);

			sprintf_s(label, "Wave %d Steepness", i);
			ImGui::SliderFloat(label, &_seaParam->waves[i].steepness, 0.0f, 300.0f);

			sprintf_s(label, "Wave %d Direction", i);
			ImGui::SliderFloat2(label, &_seaParam->waves[i].direction.x, -1.0f, 1.0f);

			ImGui::Text("\n");
		}

		if (ImGui::Button("Save to Binary File"))
		{
			char szFileName[MAX_PATH] = "";
			OPENFILENAMEA ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Save Sea Move Data";
			ofn.Flags = OFN_OVERWRITEPROMPT;

			if (GetSaveFileNameA(&ofn))
			{
				std::ofstream file(szFileName, std::ios::binary);
				if (file.is_open())
				{
					file.write(reinterpret_cast<const char*>(&_seaParam->wave_count), sizeof(_seaParam->wave_count));
					for (int i = 0; i < _seaParam->wave_count; i++)
					{
						file.write(reinterpret_cast<const char*>(&_seaParam->waves[i]), sizeof(Wave));
					}
					file.close();
				}
				else
				{
					MessageBoxA(nullptr, "파일 열기에 실패했습니다.", "Error", MB_OK | MB_ICONERROR);
				}
			}
		}
	}

	ImGui::End();
}
void ImguiManager::Debug()
{
	ImGui::Begin("Debug");

	if (ImGui::Button("Debug_ON"))
	{
		Gizmo::main->SetDebugOn(!Gizmo::main->GetDebugOn());
	}

	ImGui::End();

}
void ImguiManager::SeaColor()
{
	ImGui::Begin("sea_color");

	if (_seaParam)
	{
		ImGui::SliderFloat3("SeaBaseColor", &_seaParam->seaBaseColor.x, 0, 1.0f);
		ImGui::SliderFloat3("seaShallowColor", &_seaParam->seaShallowColor.x, 0, 1.0f);

		ImGui::SliderFloat("blendingFact", &_seaParam->blendingFact, 0, 100);
		ImGui::SliderFloat3("diffuseColor", &_seaParam->diffuseColor.x, 0, 1.0f);
		ImGui::SliderFloat("specularPower", &_seaParam->specularPower, 0, 512.0f);
		ImGui::SliderFloat3("sun_dir", &_seaParam->sun_dir.x, -1.0f, 1.0f);
		ImGui::SliderFloat("env_power", &_seaParam->env_power, 0, 1.0f);
	}

	if (ImGui::Button("Save to Binary File"))
	{
		if (_seaParam)
		{

			char szFileName[MAX_PATH] = "";
			OPENFILENAMEA ofn;
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = nullptr;
			ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrTitle = "Save Sea Move Data";
			ofn.Flags = OFN_OVERWRITEPROMPT;

			if (GetSaveFileNameA(&ofn))
			{
				std::ofstream file(szFileName, std::ios::binary);
				if (file)
				{
					file.write(reinterpret_cast<const char*>(&_seaParam->seaBaseColor), sizeof(_seaParam->seaBaseColor));
					file.write(reinterpret_cast<const char*>(&_seaParam->seaShallowColor), sizeof(_seaParam->seaShallowColor));
					file.write(reinterpret_cast<const char*>(&_seaParam->blendingFact), sizeof(_seaParam->blendingFact));
					file.write(reinterpret_cast<const char*>(&_seaParam->diffuseColor), sizeof(_seaParam->diffuseColor));
					file.write(reinterpret_cast<const char*>(&_seaParam->specularPower), sizeof(_seaParam->specularPower));
					file.write(reinterpret_cast<const char*>(&_seaParam->sun_dir), sizeof(_seaParam->sun_dir));
					file.write(reinterpret_cast<const char*>(&_seaParam->env_power), sizeof(_seaParam->env_power));
					file.close();
				}
				else
				{
					MessageBoxA(nullptr, "파일 열기에 실패했습니다.", "Error", MB_OK | MB_ICONERROR);
				}
			}
		}

	}


	ImGui::End();
}
void ImguiManager::Lighting()
{
	ImGui::Begin("LightingTest");

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	if (_light != nullptr)
	{
		vec3 tempPos = _light->_transform->GetLocalPosition();

		if (ImGui::SliderFloat3("LightPos", &tempPos.x, -300.0f, 300.0f))
		{
			_light->_transform->SetLocalPosition(tempPos);
		}
	}


	ImGui::SliderFloat3("LightDirection", &LightManager::main->_lightParmas.light[0].direction.x, -1.0f, 1.0f);
	ImGui::SliderFloat3("DiffuseColor", &LightManager::main->_lightParmas.light[0].material.diffuse.x, 0, 1.0f);
	ImGui::SliderFloat3("AmbientColor", &LightManager::main->_lightParmas.light[0].material.ambient.x, 0, 1.0f);
	ImGui::SliderFloat3("SpecularColor", &LightManager::main->_lightParmas.light[0].material.specular.x, 0, 1.0f);
	ImGui::SliderFloat("SpecularPower", &LightManager::main->_lightParmas.light[0].material.shininess, 0, 512.0f);




	ImGui::End();
}
void ImguiManager::Compute()
{
	ImGui::Begin("Compute");

	if (ImGui::Button("Blur ON/OFF"))
	{
		*_blurPtr = !(*_blurPtr);
	}

	if (ImGui::Button("Bloom ON/OFF"))
	{
		*_bloomPtr = !(*_bloomPtr);
	}

	static bool showDepthRender = false;
	if (ImGui::Button("DepthRender"))
	{
		showDepthRender = !showDepthRender;  
	}

	if (showDepthRender)
	{
		ImGui::SliderInt("renderType", &_fogParam->depthRendering, 0, 2);
		ImGui::SliderFloat("fog_Max", &_fogParam->g_fogMax, 0, 5000.0f);
		ImGui::SliderFloat("fog_Min", &_fogParam->g_fogMin, 0, 5000.0f);
		ImGui::SliderFloat3("fog_color", &_fogParam->g_fogColor.x, 0, 1.0f);
		ImGui::SliderFloat("fog_power", &_fogParam->power, 0, 30.0f);
	}


	if (ImGui::Button("underWaterEffect"))
	{
	    _underWaterParam->g_on *= -1;
	}

	if (_underWaterParam->g_on==1)
	{
		ImGui::SliderFloat3("fogColor", &_underWaterParam->g_fogColor.x, 0, 1.0f);
		ImGui::SliderFloat("fogPower", &_underWaterParam->g_fog_power, 0, 30.0f);
		ImGui::SliderFloat("fog_Max", &_underWaterParam->g_fogMax, 0, 5000.0f);
		ImGui::SliderFloat("fog_Min", &_underWaterParam->g_fogMin, 0, 5000.0f);
		ImGui::SliderFloat3("underWaterColor", &_underWaterParam->g_underWaterColor.x, 0, 1.0f);
	}

	ImGui::End();
}
;