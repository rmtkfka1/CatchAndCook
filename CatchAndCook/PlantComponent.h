#pragma once
#include "Component.h"
#include "RendererBase.h"

struct PlantInfo
{
	vec4 color = vec4(1, 1, 1, 1);
	float amplitude = 0.2f;
	float frequency = 0.5f;
	float boundsCenterY;
	float boundsSizeY;
};

class PlantComponent :public Component, public RenderStructuredSetter
{
public:
	PlantComponent();
	virtual ~PlantComponent() override;
	virtual void Init() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Update2() override;
	virtual void Enable() override;
	virtual void Disable() override;
	virtual void RenderBegin() override;
	virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() override;
	void SetDestroy() override;

	virtual void SetData(StructuredBuffer* buffer = nullptr) = 0;
private:

public:
	PlantInfo _plantInfo;

};

