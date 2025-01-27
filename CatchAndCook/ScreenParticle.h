#pragma once
#include "Component.h"
#include "StructuredBuffer.h"
#include "RendererBase.h"
struct Particle
{
	vec3 position;
	vec3 velocity;
	vec3 color;
	float life = 0.0f;
	float radius = 1.0f;
};

class ScreenParticle :public Component , public RenderObjectSetter
{
public:
	virtual ~ScreenParticle();

public:

	virtual void Init() override;
	virtual void Start() override;
	virtual void Update() override;
	virtual void Update2() override;
	virtual void Enable() override;
	virtual void Disable() override;
	virtual void RenderBegin() override;
	virtual void SetDestroy() override;
	virtual void Destroy() override;

	virtual void PushData() override;
	virtual void SetData(Material* material = nullptr) override;

private:
	vector<Particle> _paticleData;
	StructuredBuffer<Particle> _paticleGpu;

};

