#pragma once
#include "ComputeManager.h"
#include "ShadowManager.h"


class GameObject;
class Material;
struct SeaParam;
struct FogParam;
struct UnderWaterParam;

class ImguiManager
{
public:
	static unique_ptr<ImguiManager> main;

	~ImguiManager();

	void Init();
	void Render();

private:
	void Debug();

	void DebugJin();

	void BoidMove();

	void Sky();

	void GizmoController();

	void ComputeController();

	void LightController();

	void SeaController();

	void Test();

	void Test2();


	

public:
	GameObject* _light;
	bool* _blurPtr;
	bool* _bloomPtr;
	FogParam* _fogParam;
	UnderWaterParam* _underWaterParam;
	SeaParam* _seaParam;
	bool* _ssaoOnOff;
	bool* _colorGradingOnOff;

	float* separationWeight;
	float* alignmentWeight;
	float* cohesionWeight;

	float* playerHeightOffset;
	float* playerForwardOffset;
	float* cameraPitchOffset;





	bool* navMesh;
	bool* mainField_bloom;
	bool* mainField_fog;
	bool* mainField_vignette;
	bool* _shadowOnOff;
	bool* _bakedGIOnOff;
	bool* mainField_total;

private:
	bool show_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
public:
	float testValue;
};

