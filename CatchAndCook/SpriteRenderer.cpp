#include "pch.h"
#include "SpriteRenderer.h"
#include "Material.h"
#include "Mesh.h"
#include "Sprite.h"
bool SpriteRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SpriteRenderer::Init()
{

}

void SpriteRenderer::Start()
{
	
}

void SpriteRenderer::Update()
{
	_sprite->Update();
}

void SpriteRenderer::Update2()
{
}

void SpriteRenderer::Enable()
{

}

void SpriteRenderer::Disable()
{

}

void SpriteRenderer::Destroy()
{

}

void SpriteRenderer::RenderBegin()
{
	SceneManager::main->GetCurrentScene()->AddRenderer(nullptr, this, RENDER_PASS::UI);
}

void SpriteRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void SpriteRenderer::DebugRendering()
{

}

void SpriteRenderer::SetDestroy()
{

}


void SpriteRenderer::Rendering(Material* material, Mesh* mesh)
{
	_sprite->Render();
}


