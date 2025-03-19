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

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Core::main->GetCmdList().Get());
}

void ImguiManager::Debug()
{
	ImGui::Begin("GUI");
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);

    if (ImGui::CollapsingHeader("Debug"))
    {
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		ImGui::Text("ForwardCount: %d", g_debug_forward_count);
        ImGui::Text("DeferredCount: %d", g_debug_deferred_count);
		ImGui::Text("ForwardCullingCount: %d", g_debug_forward_culling_count);
		ImGui::Text("DeferredCullingCount: %d", g_debug_deferred_culling_count);
        ImGui::Text("DrawCall: %d", g_debug_draw_call);
    }

	if (ImGui::CollapsingHeader("Gizmo Controller"))
	{
        GizmoController();
	}

    if (ImGui::CollapsingHeader("Object Controller"))
    {
        SeaController();
        LightController();
        BoidMove();
    };

	if (ImGui::CollapsingHeader("Compute Controller"))
	{
		ComputeController();
	}

	ImGui::End();

}

void ImguiManager::BoidMove()
{

    if (separationWeight)
    {
        if(ImGui::TreeNode("BoidMove"));
        {
            ImGui::SliderFloat("separationWeight", separationWeight, 0, 100.0f);
            ImGui::SliderFloat("alignmentWeight", alignmentWeight, 0, 100.0f);
            ImGui::SliderFloat("cohesionWeight", cohesionWeight, 0, 100.0f);
            ImGui::TreePop();
        }
    }

}

void ImguiManager::GizmoController()
{
    if (ImGui::Button("None"))
    {
        Gizmo::main->_flags = GizmoFlags::None;
    }

    if (ImGui::Button("Collision"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::Collision;
    }

    if (ImGui::Button("DivideSpace"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::DivideSpace;
    }


    if (ImGui::Button("Culling"))
    {
        Gizmo::main->_flags = Gizmo::main->_flags | GizmoFlags::Culling;
    }
}

void ImguiManager::ComputeController()
{
    if (ImGui::Button("Blur ON/OFF"))
    {
        *_blurPtr = !(*_blurPtr);
    }

    if (ImGui::Button("Bloom ON/OFF"))
    {
        *_bloomPtr = !(*_bloomPtr);
    }

    static bool showDepthRender = false;
    if (ImGui::Button("Depth Render"))
    {
        showDepthRender = !showDepthRender;
    }
    if (showDepthRender)
    {
        ImGui::SliderInt("Render Type", &_fogParam->depthRendering, 0, 2);
        ImGui::SliderFloat("Fog Max", &_fogParam->g_fogMax, 0.0f, 5000.0f);
        ImGui::SliderFloat("Fog Min", &_fogParam->g_fogMin, 0.0f, 5000.0f);
        ImGui::SliderFloat3("Fog Color", &_fogParam->g_fogColor.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Fog Power", &_fogParam->power, 0.0f, 30.0f);
    }

    if (ImGui::Button("Underwater Effect"))
    {
        _underWaterParam->g_on *= -1;
    }
    if (_underWaterParam->g_on == 1)
    {
        ImGui::SliderFloat3("Underwater Fog Color", &_underWaterParam->g_fogColor.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Underwater Fog Power", &_underWaterParam->g_fog_power, 0.0f, 30.0f);
        ImGui::SliderFloat("Underwater Fog Max", &_underWaterParam->g_fogMax, 0.0f, 5000.0f);
        ImGui::SliderFloat("Underwater Fog Min", &_underWaterParam->g_fogMin, 0.0f, 5000.0f);
        ImGui::SliderFloat3("Underwater Color", &_underWaterParam->g_underWaterColor.x, 0.0f, 1.0f);
    }
}

void ImguiManager::LightController()
{
    if (ImGui::TreeNode("Lighting"))
    {
        ImGui::SliderFloat3("Light Direction", &LightManager::main->_lightParmas.light[0].direction.x, -1.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse Color", &LightManager::main->_lightParmas.light[0].material.diffuse.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Ambient Color", &LightManager::main->_lightParmas.light[0].material.ambient.x, 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular Color", &LightManager::main->_lightParmas.light[0].material.specular.x, 0.0f, 1.0f);
        ImGui::SliderFloat("Specular Power", &LightManager::main->_lightParmas.light[0].material.shininess, 0.0f, 512.0f);
        ImGui::TreePop();
    };
}

void ImguiManager::SeaController()
{
    if (ImGui::TreeNode("Sea Controller"))
    {
        if (_seaParam)
        {
       
            if (ImGui::TreeNode("SeaMove"))
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

     
                if (ImGui::Button("Save SeaMove to Binary File"))
                {
                    char szFileName[MAX_PATH] = "";
                    OPENFILENAMEA ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr;
                    ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrTitle = "Save SeaMove Data";
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
                ImGui::TreePop(); 
            }

            if (ImGui::TreeNode("SeaColor"))
            {
                ImGui::SliderFloat3("SeaBaseColor", &_seaParam->seaBaseColor.x, 0.0f, 1.0f);
                ImGui::SliderFloat3("SeaShallowColor", &_seaParam->seaShallowColor.x, 0.0f, 1.0f);
                ImGui::SliderFloat("blendingFact", &_seaParam->blendingFact, 0.0f, 100.0f);
                ImGui::SliderFloat3("diffuseColor", &_seaParam->diffuseColor.x, 0.0f, 1.0f);
                ImGui::SliderFloat("specularPower", &_seaParam->specularPower, 0.0f, 512.0f);
                ImGui::SliderFloat3("sun_dir", &_seaParam->sun_dir.x, -1.0f, 1.0f);
                ImGui::SliderFloat("env_power", &_seaParam->env_power, 0.0f, 1.0f);

 
                if (ImGui::Button("Save SeaColor to Binary File"))
                {
                    char szFileName[MAX_PATH] = "";
                    OPENFILENAMEA ofn;
                    ZeroMemory(&ofn, sizeof(ofn));
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = nullptr;
                    ofn.lpstrFilter = "Binary Files (*.bin)\0*.bin\0All Files (*.*)\0*.*\0";
                    ofn.lpstrFile = szFileName;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.lpstrTitle = "Save SeaColor Data";
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
                ImGui::TreePop(); 
            }
        }
        ImGui::TreePop(); 
    }
}
