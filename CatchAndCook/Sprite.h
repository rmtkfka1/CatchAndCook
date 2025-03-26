#pragma once
#include "Component.h"
#include "RendererBase.h"
#include "SpriteAction.h"

class ActionCommand;

struct SpriteWorldParam
{
	vec3 ndcPos = { 0.0f,0.0f,0.1f };
	float alpha = 1.0f;

	vec2 ndcScale = { 1.0f,1.0f };
	vec2 padding = {};

	vec4 clipingColor;
};

struct SprtieTextureParam
{
	vec2 origintexSize{};
	vec2 texSamplePos{};
	vec2 texSampleSize{};
	vec2 padding;
};

struct SpriteRect
{
	float left;
	float top;
	float right;
	float bottom;
};

class Sprite :public Component, public RenderCBufferSetter
{
public:
	Sprite();
	virtual ~Sprite();

public:
	void Init() override;
	void Start() override;
	void Update()override;
	void Update2()override;
	void Enable()override;
	void Disable()override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override ;
	void SetDestroy() override;
	void Destroy() override;
	void SetData(Material* material = nullptr) override;

public:
	void SetSize(const vec2& size);
	void SetPos(const vec3& screenPos);
	void SetUVCoord(const SpriteRect& rect);
	void SetClipingColor(const vec4& color);  // https://imagecolorpicker.com/
	void AddAction(shared_ptr<ActionCommand> action) { _actions.emplace_back(action); };

private:
	SpriteWorldParam _spriteWorldParam;
	SprtieTextureParam _sprtieTextureParam;

	vec3 _screenPos;
	vec3 _ndcPos;

	vec2 _screenSize;
	vec2 _ndcSize;

	vec2 _firstWindowSize;

	friend class ActionFunc;
	vector<shared_ptr<ActionCommand>> _actions;

};

