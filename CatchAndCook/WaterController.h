#pragma once
#include "Component.h"
#include "RendererBase.h"

class Texture;

struct SeaParam
{
    vec4 seaBaseColor= vec4((27/ 255.0f, 57/ 255.0f, 77/ 255.0f,1.0f));
    vec4 seaShallowColor = vec4(75 / 255.0f, 89 / 255.0f, 35 / 255.0f, 1.0f);
};


class WaterController :public Component , public RenderCBufferSetter
{

public:
    ~WaterController() override;
    bool IsExecuteAble() override;

    virtual void Init();
    virtual void Start();
    virtual void Update();
    virtual void Update2();
    virtual void Enable();
    virtual void Disable();
    virtual void RenderBegin();
    virtual void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
    virtual void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
    virtual void SetDestroy() override;
    virtual void Destroy();

    virtual void SetData(Material* material = nullptr);

private:
    shared_ptr<Texture> _textures;
    shared_ptr<Texture> _bump;

	CBufferContainer* _cbufferContainer;

    SeaParam _seaParam;

};

