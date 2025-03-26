#include "pch.h"
#include "Inventory.h"

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

void Inventory::Init()
{
	
}

void Inventory::Start()
{
}

void Inventory::Update()
{
}

void Inventory::Update2()
{
}

void Inventory::Enable()
{
}

void Inventory::Disable()
{
}

void Inventory::RenderBegin()
{
}

void Inventory::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Inventory::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Inventory::SetDestroy()
{
}

void Inventory::Destroy()
{
}

void Inventory::SetData(Material* material)
{

}

void Inventory::SetPos(const vec3& pos)
{
	_InventorySprite->SetLocalPos(pos);
}

void Inventory::SetSize(const vec2& size)
{
	_InventorySprite->SetSize(size);
}


