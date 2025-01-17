#include "pch.h"
#include "Sprite.h"
#include "Mesh.h"

vector<pair<CollisionRect, Sprite*>> Sprite::_collisionMap;

Sprite::Sprite()
{
	Init();
}

Sprite::~Sprite()
{

	auto it = std::remove_if(_collisionMap.begin(), _collisionMap.end(),
		[this](const auto& pair) {
			return pair.second == this;
		});

	if (it != _collisionMap.end())
	{
		_collisionMap.erase(it);
	}

}

void Sprite::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_shader = ResourceManager::main->Get<Shader>(L"SpriteShader");
}

void Sprite::Update()
{
	
	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		auto pos = Input::main->GetMouseDownPosition(KeyCode::LeftMouse);
		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		for (auto& [rect, sprite] : _collisionMap)
		{
			if (normalizedX >= rect.left  && normalizedX <= rect.right  &&
				normalizedY >= rect.top  && normalizedY <= rect.bottom )
			{
				sprite->_spriteParam.alpha *= 0.99f;
			}
		}
	}


}

void Sprite::Render()
{
	auto& cmdList = Core::main->GetCmdList();

	cmdList->SetPipelineState(_shader->_pipelineState.Get());

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
	_spriteParam.ndcScale.x = size.x / WINDOW_WIDTH;
	_spriteParam.ndcScale.y = size.y / WINDOW_HEIGHT;

	_ndcSize = _spriteParam.ndcScale;
}

void Sprite::SetPos(vec3 pos)
{
	_spriteParam.ndcPos.x = pos.x/ WINDOW_WIDTH;
	_spriteParam.ndcPos.y = pos.y/ WINDOW_HEIGHT;
	_spriteParam.ndcPos.z = pos.z;

	_ndcPos = _spriteParam.ndcPos;
}

void Sprite::AddCollisonMap()
{
	CollisionRect rect;
	rect.left =  (_ndcPos.x);
	rect.top  =  (_ndcPos.y);
	rect.right = (_ndcPos.x + _ndcSize.x);
	rect.bottom = (_ndcPos.y + _ndcSize.y);

	_collisionMap.push_back({ rect, this });
}




void Sprite::SetTexture(shared_ptr<Texture> texture, RECT* rect)
{
	_texture = texture;

	auto desc = _texture->GetResource()->GetDesc();

	_spriteParam.origintexSize = vec2(desc.Width, desc.Height);

	if (rect)
	{
		_spriteParam.texSamplePos.x = rect->left;
		_spriteParam.texSamplePos.y = rect->top;
		_spriteParam.texSampleSize.x = (rect->right-rect->left);
		_spriteParam.texSampleSize.y = (rect->bottom - rect->top);
	}
	else
	{

		_spriteParam.texSamplePos.x = 0;
		_spriteParam.texSamplePos.y = 0;
		_spriteParam.texSampleSize.x = desc.Width;
		_spriteParam.texSampleSize.y = desc.Height;
	}
}

/// <summary>
/// //
/// </summary>

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
