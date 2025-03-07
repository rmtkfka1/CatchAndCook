#pragma once

class GameObject;
class Material;
class ImguiManager
{
public:
	static unique_ptr<ImguiManager> main;

	~ImguiManager();

	void Init();
	void Render();

public:
	GameObject* _light;
	Material* _seaMaterial;
	bool* _blurPtr;

private:
	bool show_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

