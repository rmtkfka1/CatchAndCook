#pragma once

#include "Component.h"
#include "RendererBase.h"

class Mesh;

struct SpriteParam
{
	vec3 pos = { 0.0f,0.0f,0.1f };
	float alpha;

	vec2 scale = { 1.0f,1.0f };
	vec2 origintexSize;

	vec2 texSamplePos;
	vec2 texSampleSize;


};

class SpriteRenderer : public Component, public RendererBase
{

public:
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void Destroy() override;
	void RenderBegin() override;
	void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void DebugRendering() override;
	void SetDestroy() override;
	void DestroyComponentOnly() override;

	void Rendering(const std::shared_ptr<Material>& material) override;

	

public:
	void SetTexture(shared_ptr<Texture> texture, RECT* rect = nullptr);
	void SetSize(vec2 size);
	void SetPos(vec3 pos);

private:
	tableContainer _tableContainer;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;
	shared_ptr<Shader> _shader;
	SpriteParam _spriteParam;

	RECT _rect{0,0,0,0}; //uvMapping 에 사용됨.

public:
	int _temp = 0; //임시적으로 윈도우사이즈 맞게 변경되야해서 사용됨.
};

