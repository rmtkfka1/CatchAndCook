#pragma once
#include "Component.h"
class testComponent : public Component
{

public:
	virtual ~testComponent();

	virtual void Init();
	virtual void Start();
	virtual void Update();
	virtual void Update2();
	virtual void Enable();
	virtual void Disable();
	virtual void Destroy();
	virtual void RenderBegin();
	virtual void Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	virtual void SetDestroy() override;
	virtual void DestroyComponentOnly();



private:

};

