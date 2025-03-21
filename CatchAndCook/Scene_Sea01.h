#pragma once
#include "Scene.h"
class Scene_Sea01 :public Scene
{
public:
	void Init() override;
	void Update() override;
	void RenderBegin() override;
	void Rendering() override;
	void DebugRendering() override;
	void RenderEnd() override;
	void Finish() override;



};

