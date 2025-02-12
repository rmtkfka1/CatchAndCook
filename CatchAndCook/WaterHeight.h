#pragma once
#include "Component.h"

#define PI 3.14159f

struct WaveParams 
{
    float amplitude;
    float wavelength;
    float speed;
    vec2 direction;
};


class WaterHeight : public Component
{
public:
    ~WaterHeight() override;
    bool IsExecuteAble() override;
    virtual void Init();
    virtual void Start();
    virtual void Update();
    virtual void Update2();
    virtual void Enable();
    virtual void Disable();
    virtual void RenderBegin();
    virtual void CollisionBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);
    void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
    virtual void SetDestroy() override;
    virtual void Destroy();

    float GetWaveHeight(float x,float z,float time);
    void  SetOffset(float offset) {_offset =offset;}

private:

    float _offset=0;

    WaveParams waves[3] =
    {
        {11.0f,500.0f,0.5f,vec2(1.0f,0.2f)},
        {9.0f,300.0f,1.0f,vec2(0.3f,-1.0f)},
        {7.0f,200.0f,0.8f,vec2(-0.5f,0.7f)}
    };

};

