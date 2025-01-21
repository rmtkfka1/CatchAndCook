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

	TestMouseLeftUpdate();
	TestMouseRightUpdate();

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
	auto tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(&tableContainer.CPUHandle, &_texture->GetSRVCpuHandle(), 0);
	cmdList->SetGraphicsRootDescriptorTable(SPRITE_TABLE_INDEX, tableContainer.GPUHandle);

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
	_screenSize = size;
}

void Sprite::SetPos(vec3 screenPos)
{
	_spriteParam.ndcPos.x = screenPos.x/ WINDOW_WIDTH;
	_spriteParam.ndcPos.y = screenPos.y/ WINDOW_HEIGHT;
	_spriteParam.ndcPos.z = screenPos.z;

	_ndcPos = _spriteParam.ndcPos;
	_screenPos = screenPos;

}

void Sprite::AddCollisonMap()
{
	CollisionRect rect;
	rect.left =  (_spriteParam.ndcPos.x);
	rect.top  =  (_spriteParam.ndcPos.y);
	rect.right = (_spriteParam.ndcPos.x + _spriteParam.ndcScale.x);
	rect.bottom = (_spriteParam.ndcPos.y + _spriteParam.ndcScale.y);

	_collisionMap.push_back({ rect, this });

	std::sort(_collisionMap.begin(), _collisionMap.end(), [](const auto& a, const auto& b) {
		return a.second->_spriteParam.ndcPos.z < b.second->_spriteParam.ndcPos.z;
		});

}

void Sprite::TestMouseLeftUpdate()
{
	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		auto pos = Input::main->GetMouseDownPosition(KeyCode::LeftMouse);
		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		for (auto& [rect, sprite] : _collisionMap)
		{
			if (normalizedX >= rect.left && normalizedX <= rect.right &&
				normalizedY >= rect.top && normalizedY <= rect.bottom)
			{
				sprite->_spriteParam.alpha *= 0.99f;
				break;
			}
		}
	}
}

void Sprite::TestMouseRightUpdate()
{
	static Sprite* _dragSprtie = nullptr;
	static CollisionRect* _dragRect = nullptr;

	// 우클릭 시작
	if (Input::main->GetMouseDown(KeyCode::RightMouse))
	{
		vec2 pos = Input::main->GetMouseDownPosition(KeyCode::RightMouse);
		float normalizedX = static_cast<float>(pos.x) / WINDOW_WIDTH;
		float normalizedY = static_cast<float>(pos.y) / WINDOW_HEIGHT;

		// 충돌된 스프라이트 검색
		for (auto& [rect, sprite] : _collisionMap)
		{
			if (normalizedX >= rect.left && normalizedX <= rect.right &&
				normalizedY >= rect.top && normalizedY <= rect.bottom)
			{
				_dragSprtie = sprite;
				_dragRect = &rect;
				break; // 첫 번째로 충돌된 스프라이트만 선택
			}
		}
	}

	// 드래그 중
	if (_dragSprtie && Input::main->GetMouse(KeyCode::RightMouse))
	{
		vec2 pos = Input::main->GetMousePosition(); 
		auto size = _dragSprtie->_screenSize;
		_dragSprtie->SetPos(vec3(pos.x - size.x/2, pos.y-size.y/2, _dragSprtie->_spriteParam.ndcPos.z));
	}

	// 우클릭 종료
	if (_dragSprtie && Input::main->GetMouseUp(KeyCode::RightMouse))
	{
		_dragRect->left = _dragSprtie->_ndcPos.x;
		_dragRect->top = _dragSprtie->_ndcPos.y;
		_dragRect->right = _dragSprtie->_ndcPos.x + _dragSprtie->_ndcSize.x;
		_dragRect->bottom = _dragSprtie->_ndcPos.y + _dragSprtie->_ndcSize.y;

		// 드래그 상태 종료
		_dragSprtie = nullptr;
		_dragRect = nullptr;
	};
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
