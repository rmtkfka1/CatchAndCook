#include "pch.h"
#include "SpriteRenderer.h"

bool SpriteRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SpriteRenderer::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_material = make_shared<Material>(false);
	_material->SetShader(ResourceManager::main->Get<Shader>(L"Uishader"));
}

void SpriteRenderer::Start()
{

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
	_material->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
	_material->PushData();

	SceneManager::main->GetCurrentScene()->AddRenderer(_material, static_pointer_cast<SpriteRenderer>(shared_from_this()));
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
	if (material != nullptr)
		material->SetData();

	auto& cmdList = Core::main->GetCmdList();


}
