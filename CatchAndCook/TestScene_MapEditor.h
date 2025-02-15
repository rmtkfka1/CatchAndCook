#pragma once
class TestScene_MapEditor: public Scene
{
public:
	void Init() override;
	void Update() override;
	void RenderBegin() override;
	void Rendering() override;
	void DebugRendering() override;
	void RenderEnd() override;
	void Finish() override;
	~TestScene_MapEditor() override;
};

