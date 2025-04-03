#pragma once
#include "Component.h"
#include "RendererBase.h"

struct PlantInfo
{
	float amplitude;   // 흔들림의 강도
	float frequency;   // 흔들림의 빈도
	float padding;
	float padding2;
};

class PlantComponent :public Component, public RenderCBufferSetter
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
	virtual void SetData(Material* material = nullptr);
private:

public:
	PlantInfo _plantInfo;

};

