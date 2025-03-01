#include "pch.h"
#include "ImguiManager.h"

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

		ImGui::End(); // 반드시 End()를 호출해야 UI 레이아웃이 정상적으로 유지됨
	}

	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), Core::main->GetCmdList().Get());
};