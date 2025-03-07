#pragma once
#include "Component.h"
#include "RendererBase.h"

class Texture;


struct Wave
{
    float amplitude;
    float wavelength;
    float speed;
    float steepness;

    vec2 direction;
    vec2 padding;
};


struct SeaParam
{
    vec4 seaBaseColor= vec4(0.566f,0.966f,1.000f,1.0f);
    vec4 seaShallowColor = vec4(0.361f, 0.675f, 0.706f,1.0f);
    float blendingFact = float(20.619);
    vec3 diffuseColor = vec3(1.0f, 0.284f, 0.162f);

    float specularPower = float(207.0f);
    vec3 sun_dir = vec3(-1.0f,-0.045f,0.647f);

    float env_power = 0.3f;
    float wave_count = 3;
    vec2 padding;

	Wave waves[3];
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
    shared_ptr<Texture> _dudv;

	CBufferContainer* _cbufferContainer;

    SeaParam _seaParam;

};

