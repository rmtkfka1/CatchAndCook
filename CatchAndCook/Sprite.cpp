#include "pch.h"
#include "Sprite.h"
#include "Mesh.h"


Sprite::Sprite()
{
	Init();
}

Sprite::~Sprite()
{
}

void Sprite::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_shader = ResourceManager::main->Get<Shader>(L"SpriteShader");
}

void Sprite::Update()
{
	SetPos(vec3(0 + _temp * WINDOW_WIDTH / 5, 0, 0.99f));
	SetSize(vec2(WINDOW_WIDTH / 5, WINDOW_HEIGHT / 3));
}

void Sprite::Render()
{
	auto& cmdList = Core::main->GetCmdList();

	cmdList->SetPipelineState(_shader->_pipelineState.Get());

	_spriteParam.texSamplePos.x = _rect.left;
	_spriteParam.texSamplePos.y = _rect.top;
	_spriteParam.texSampleSize.x = _rect.right - _rect.left;
	_spriteParam.texSampleSize.y = _rect.bottom - _rect.top;

	//스프라이트 파람 바인딩.
	auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteParam)->Alloc(1);
	memcpy(CbufferContainer->ptr, (void*)&_spriteParam, sizeof(SpriteParam));
	cmdList->SetGraphicsRootConstantBufferView(5, CbufferContainer->GPUAdress);

	//텍스쳐 바인딩.
	_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(&_tableContainer.CPUHandle, &_texture->GetSRVCpuHandle(), 0);
	cmdList->SetGraphicsRootDescriptorTable(SPRITE_TABLE_INDEX, _tableContainer.GPUHandle);

	cmdList->IASetPrimitiveTopology(_mesh->GetTopology());

	if (_mesh->GetVertexCount() != 0)
	{

		if (_mesh->GetIndexCount() != 0)
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->IASetIndexBuffer(&_mesh->GetIndexView());
			cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
		}
		else
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->DrawInstanced(_mesh->GetVertexCount(), 1, 0, 0);
		}
	}
}

void Sprite::SetSize(vec2 size)
{
	if (_texture == nullptr)
		assert(false);

	auto desc = _texture->GetResource()->GetDesc();

	_spriteParam.scale.x = size.x / WINDOW_WIDTH;
	_spriteParam.scale.y = size.y / WINDOW_HEIGHT;

}

void Sprite::SetPos(vec3 pos)
{
	_spriteParam.pos = pos;
}

void Sprite::SetTexture(shared_ptr<Texture> texture, RECT* rect)
{
	_texture = texture;

	auto desc = _texture->GetResource()->GetDesc();

	_spriteParam.origintexSize = vec2(desc.Width, desc.Height);

	if (rect)
	{
		_rect = *rect;
	}
	else
	{
		_rect.left = 0;
		_rect.top = 0;
		_rect.right = desc.Width;
		_rect.bottom = desc.Height;
	}
}

//

void Inventory::Init()
{

}

void Inventory::Update()
{
	Sprite::Update();
}

void Inventory::Render()
{
	Sprite::Render();
}
