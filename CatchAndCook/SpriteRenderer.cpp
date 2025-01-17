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
	_sprite->Update();
}

void SpriteRenderer::Update()
{


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
	SceneManager::main->GetCurrentScene()->AddRenderer(static_pointer_cast<SpriteRenderer>(shared_from_this()), RENDER_PASS::UI);
}

void SpriteRenderer::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void SpriteRenderer::DebugRendering()
{

}

void SpriteRenderer::SetDestroy()
{

}

void SpriteRenderer::DestroyComponentOnly()
{

}

void SpriteRenderer::Rendering(const std::shared_ptr<Material>& material)
{
	_sprite->Render();
}


