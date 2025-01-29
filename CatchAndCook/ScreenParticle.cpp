#include "pch.h"
#include "ScreenParticle.h"
#include <random>
#include "GameObject.h"
ScreenParticle::~ScreenParticle()
{
}

void ScreenParticle::Init()
{
	_paticleData.resize(2048);

    vector<vec3> rainbow = 
    {
        {1.0f, 0.0f, 0.0f}, 
        {1.0f, 0.65f, 0.0f},
        {1.0f, 1.0f, 0.0f}, 
        {0.0f, 1.0f, 0.0f}, 
        {0.0f, 0.0f, 1.0f}, 
        {0.3f, 0.0f, 0.5f}, 
        {0.5f, 0.0f, 1.0f}  
    };

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> dp(-1.0f, 1.0f);
    uniform_int_distribution<size_t> dc(0, rainbow.size() - 1);

    for (auto& p : _paticleData)
    {
        p.position = vec3(dp(gen), dp(gen), 1.0f);
        p.color = rainbow[dc(gen)];
        p.radius = (dp(gen) + 1.3f) * 0.02f;
        p.life = -1.0f;
    }

    _paticleGpu.Init(_paticleData);
}

void ScreenParticle::Start()
{
    if (GetOwner()->GetRenderer())
    {
        GetOwner()->GetRenderer()->AddSetter(static_pointer_cast<ScreenParticle>(shared_from_this()));
    }

}

void ScreenParticle::Update()
{
    float dt = Time::main->GetDeltaTime();

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<float> randomTheta(-3.141592f, 3.141592f);
    uniform_real_distribution<float> randomSpeed(1.5f, 2.0f);
    uniform_real_distribution<float> randomLife(0.0f, 1.0f);

    // 마우스 클릭시 추가
    int newCount = 50; 
    for (auto& p : _paticleData)
    {
        if (Input::main->GetMouse(KeyCode::RightMouse) && p.life < 0.0f && newCount >0)
        {
            const float theta = randomTheta(gen);

            auto pos = Input::main->GetMousePosition();

            p.position = vec3(0, 0, 0.0);

            p.velocity = vec3(cos(theta), -sin(theta), 0.0) * randomSpeed(gen);

            p.life = randomLife(gen) * 1.5f;

            newCount--;
        }
    }

    const vec3 gravity = vec3(0.0f, -9.8f, 0.0f);
    const float cor = 0.5f; 
    const float groundHeight = -1.0f;

    for (auto& p : _paticleData)
    {
        if (p.life < 0.0f) 
            continue;

        p.velocity = p.velocity + gravity * dt;
        p.position += p.velocity * dt;
        p.life -= dt;

        if (p.position.y < groundHeight && p.velocity.y < 0.0f)
        {
            p.velocity.y *= -randomLife(gen);
            p.position.y = groundHeight;
        }

        if (p.position.x < -1.0f && p.velocity.x < 0.0f) {
            p.velocity.x *= -randomLife(gen);
            p.position.x = -1.0f;
        }

        if (p.position.x > 1.0f && p.velocity.x > 0.0f) {
            p.velocity.x *= -randomLife(gen);
            p.position.x = 1.0f;
        }

    }

    _paticleGpu.Upload(_paticleData);
}

void ScreenParticle::Update2()
{
}

void ScreenParticle::Enable()
{
}

void ScreenParticle::Disable()
{
}

void ScreenParticle::RenderBegin()
{
}

void ScreenParticle::SetDestroy()
{
}

void ScreenParticle::Destroy()
{
    if (GetOwner()->GetRenderer())
    {
        GetOwner()->GetRenderer()->RemoveSetters(static_pointer_cast<ScreenParticle>(shared_from_this()));
    }
}

void ScreenParticle::PushData()
{

}

void ScreenParticle::SetData(Material* material)
{
    material->SetHandle("particles", _paticleGpu.GetSRVHandle());
}

