#include "pch.h"
#include "ImguiManager.h"
#include "GameObject.h"
#include "Transform.h"
#include "LightManager.h"
#include "WaterController.h"
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
		if (ImGui::Button("Blur ON/OFF"))
		{
			*_blurPtr = !(*_blurPtr);
		}

		ImGui::End();
	}

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

		ImGui::End();
	}

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
				ImGui::SliderFloat(label, &_seaParam->waves[i].steepness, 0.0f, 1.0f);

				sprintf_s(label, "Wave %d Direction", i);
				ImGui::SliderFloat2(label, &_seaParam->waves[i].direction.x, -1.0f, 1.0f);

				ImGui::Text("\n");
			}

			if (ImGui::Button("Save to Binary File"))
			{
				std::ofstream file("../Resources/Textures/sea/sea_move.bin", std::ios::binary);
				if (file.is_open())
				{
		
					file.write(reinterpret_cast<const char*>(&_seaParam->wave_count), sizeof(_seaParam->wave_count));

					for (int i = 0; i < _seaParam->wave_count; i++)
					{
						file.write(reinterpret_cast<const char*>(&_seaParam->waves[i]), sizeof(Wave));
					}

					file.close();
				}
			}
		}
		ImGui::End();
	}
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Core::main->GetCmdList().Get());
};