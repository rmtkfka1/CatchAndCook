#include "pch.h"
#include "SpriteRenderer.h"

bool SpriteRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SpriteRenderer::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_shader = ResourceManager::main->Get<Shader>(L"SprtieShader");
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
	SceneManager::main->GetCurrentScene()->AddRenderer(nullptr, static_pointer_cast<SpriteRenderer>(shared_from_this()));
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
	auto& cmdList = Core::main->GetCmdList();

	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	
	if (_rect.left == 0 && _rect.right == 0 && _rect.bottom == 0 && _rect.top == 0)
	{
		auto desc = _texture->GetResource()->GetDesc();

		_spriteParam.texSamplePos.x = _rect.left;
		_spriteParam.texSamplePos.y = _rect.top;
		_spriteParam.texSampleSize.x = desc.Width;
		_spriteParam.texSampleSize.y = desc.Height;
	}

	else
	{
		_spriteParam.texSamplePos.x = _rect.left;
		_spriteParam.texSamplePos.y = _rect.top;
		_spriteParam.texSampleSize.x = _rect.right - _rect.left;
		_spriteParam.texSampleSize.y = _rect.top - _rect.bottom;
	}

	//스프라이트 파람 바인딩.

	auto CbufferContainer= Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteParam)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_spriteParam, sizeof(SpriteParam));
	cmdList->SetGraphicsRootConstantBufferView(5, CbufferContainer->GPUAdress);

	//텍스쳐 바인딩.
	_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(&_tableContainer.cpuHandle, &_texture->GetSRVCpuHandle(), 0);
	cmdList->SetComputeRootDescriptorTable(SPRITE_TABLE_INDEX, _tableContainer.gpuHandle);


}

void SpriteRenderer::SetSize(vec2 size)
{
	if (_texture == nullptr)
		assert(false);

	auto desc = _texture->GetResource()->GetDesc();

	_spriteParam.Scale.x = size.x /WINDOW_WIDTH;
	_spriteParam.Scale.x = size.y / WINDOW_HEIGHT;

}

void SpriteRenderer::SetPos(vec3 pos)
{
	_spriteParam.Pos.x = pos.x;
	_spriteParam.Pos.y = pos.y;
	_spriteParam.depth = pos.z;
}

void SpriteRenderer::SetTexture(shared_ptr<Texture> texture, RECT* rect)
{
	_texture = texture;

	auto desc = _texture->GetResource()->GetDesc();
	
	_spriteParam.origintexSize = vec2(desc.Width, desc.Height);

	if (rect)
	{
		_rect = *rect;
	}
}

