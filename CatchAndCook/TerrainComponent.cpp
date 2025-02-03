#include "pch.h"
#include "TerrainComponent.h"

TerrainComponent::~TerrainComponent()
{
}

bool TerrainComponent::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void TerrainComponent::Init()
{
	Component::Init();
}

void TerrainComponent::Start()
{
	Component::Start();
}

void TerrainComponent::Update()
{
	Component::Update();
}

void TerrainComponent::Update2()
{
	Component::Update2();
}

void TerrainComponent::Enable()
{
	Component::Enable();
}

void TerrainComponent::Disable()
{
	Component::Disable();
}

void TerrainComponent::RenderBegin()
{
	Component::RenderBegin();
}

void TerrainComponent::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider,other);
}

void TerrainComponent::SetDestroy()
{
	Component::SetDestroy();
}

void TerrainComponent::Destroy()
{
	Component::Destroy();
}

float TerrainComponent::GetHeight(Vector3 localPosition)
{
	Vector2 uv = Vector2(localPosition.x,localPosition.z) / (_terrain->_heightSize - Vector2::One);
	//uv.y = 1 - uv.y;
	uv.Clamp(Vector2::Zero,Vector2::One,uv);

	int minX = ((int)uv.x);
	int minY = ((int)uv.y);
	int maxX = minX + 1;
	int maxY = minY + 1;

	//GetHeightXY(minXx,) ≈Õ∑π¿Œ ¿€æ˜¡ﬂø° ∏ÿ√Ë¿Ω
	return 1;
}
