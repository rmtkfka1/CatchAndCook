#include "pch.h"
#include "Sprite.h"
#include "Mesh.h"
#include "SpriteAction.h"
#include "TextManager.h"

Sprite::Sprite()
{
	_firstWindowSize = vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
}

Sprite::~Sprite()
{

}

void Sprite::SetSize(vec2 size)
{
	_spriteWorldParam.ndcScale.x = size.x / _firstWindowSize.x;
	_spriteWorldParam.ndcScale.y = size.y / _firstWindowSize.y;

	_ndcSize = _spriteWorldParam.ndcScale;
	_screenSize = size;
}

void Sprite::SetPos(vec3 screenPos)
{
	_spriteWorldParam.ndcPos.x = screenPos.x/ _firstWindowSize.x;
	_spriteWorldParam.ndcPos.y = screenPos.y/ _firstWindowSize.y;
	_spriteWorldParam.ndcPos.z = screenPos.z;

	_ndcPos = _spriteWorldParam.ndcPos;
	_screenPos = screenPos;

}

void Sprite::SetClipingColor(vec4 color)
{
	_spriteWorldParam.clipingColor = color;
}

void Sprite::AddAction(shared_ptr<ActionCommand> action)
{
	_actions.push_back(action);
}
void Sprite::AddChildern(shared_ptr<Sprite> child)
{
	_children.push_back(child);
	child->_parent = shared_from_this();
}

BasicSprite::BasicSprite()
{
	Init();
}

BasicSprite::~BasicSprite()
{

}

void BasicSprite::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_shader = ResourceManager::main->Get<Shader>(L"SpriteShader");
}

void BasicSprite::Update()
{

	for (auto& action : _actions)
	{
		action->Execute(this);
	}

	for (auto& ele : _children)
	{
		for (auto& action : ele->_actions)
		{
			action->Execute(ele.get());
		}
	}
}

void BasicSprite::Render()
{
	if (_renderEnable == false)
		return;

	auto& cmdList = Core::main->GetCmdList();

	// ���������� ���� ����
	cmdList->SetPipelineState(_shader->_pipelineState.Get());

	// SpriteWorldParam ���� ����
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(_shader->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	// SpriteTextureParam ���� ����
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam, sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(_shader->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}

	// �ؽ��� ���ε�
	auto tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(tableContainer.CPUHandle, _texture->GetSRVCpuHandle(), 0);
	cmdList->SetGraphicsRootDescriptorTable(SPRITE_TABLE_INDEX, tableContainer.GPUHandle);

	// �޽� ����
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

	for (auto& child : _children)
	{
		child->Render();
	}
}

void BasicSprite::SetUVCoord(SpriteRect& rect)
{
	_sprtieTextureParam.texSamplePos.x = rect.left;
	_sprtieTextureParam.texSamplePos.y = rect.top;
	_sprtieTextureParam.texSampleSize.x = (rect.right - rect.left);
	_sprtieTextureParam.texSampleSize.y = (rect.bottom - rect.top);
}

void BasicSprite::SetTexture(shared_ptr<Texture> texture)
{
	_texture = texture;

	auto desc = _texture->GetResource()->GetDesc();

	_sprtieTextureParam.origintexSize = vec2(desc.Width, desc.Height);

	_sprtieTextureParam.texSamplePos.x = 0;
	_sprtieTextureParam.texSamplePos.y = 0;
	_sprtieTextureParam.texSampleSize.x = desc.Width;
	_sprtieTextureParam.texSampleSize.y = desc.Height;
}



/*****************************************************************
*                                                                *
*                         AnimationSprite                        *
*                                                                *
******************************************************************/


AnimationSprite::AnimationSprite()
{
	Init();
}

AnimationSprite::~AnimationSprite()
{

}

void AnimationSprite::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_shader = ResourceManager::main->Get<Shader>(L"SpriteShader");
} 

void AnimationSprite::Update()
{

	for (auto& action : _actions)
	{
		action->Execute(this);
	}

	for (auto& child : _children)
	{
		for (auto& action : child->_actions)
		{
			action->Execute(child.get());
		}
	}

	AnimationUpdate();

}

void AnimationSprite::Render()
{
	if (_renderEnable == false)
		return;


	auto& cmdList = Core::main->GetCmdList();

	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(_shader->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam[_currentFrameIndex], sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(_shader->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}

	//�ؽ��� ���ε�.
	auto tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(tableContainer.CPUHandle, _texture->GetSRVCpuHandle(), 0);
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

	for (auto& child : _children)
	{
		child->Render();
	}


}

void AnimationSprite::PushUVCoord(SpriteRect& rect)
{
	if (_texture == nullptr)
		assert(false);

	SprtieTextureParam sprtieTextureParam;

	auto desc = _texture->GetResource()->GetDesc();

	sprtieTextureParam.origintexSize = vec2(desc.Width, desc.Height);
	sprtieTextureParam.texSamplePos.x = rect.left;
	sprtieTextureParam.texSamplePos.y = rect.top;
	sprtieTextureParam.texSampleSize.x = (rect.right - rect.left);
	sprtieTextureParam.texSampleSize.y = (rect.bottom - rect.top);

	_sprtieTextureParam.push_back(sprtieTextureParam);

	_maxFrameIndex += 1;
}

void AnimationSprite::SetTexture(shared_ptr<Texture> texture)
{
	_texture = texture;

}


void AnimationSprite::AnimationUpdate()
{
	float dt = Time::main->GetDeltaTime();
	_currentTime += dt; 

	if (_currentTime >= _frameRate) 
	{ 
		_currentTime -= _frameRate; 

		_currentFrameIndex++; 

		if (_currentFrameIndex >= _maxFrameIndex)
		{
			_currentFrameIndex = 0; 
		}
	}
}

TextSprite::TextSprite()
{
	Init();
}

TextSprite::~TextSprite()
{
}

void TextSprite::Init()
{
	_mesh = GeoMetryHelper::LoadSprtieMesh();
	_shader = ResourceManager::main->Get<Shader>(L"SpriteShader");

	int g_ImageWidth = 512;
	int g_ImageHeight = 256;

	testptr = new BYTE[(g_ImageWidth * g_ImageHeight * 4)];

	DWORD* pDest = (DWORD*)testptr;
	for (DWORD y = 0; y < g_ImageHeight; y++)
	{
		for (DWORD x = 0; x < g_ImageWidth; x++)
		{
			pDest[x + g_ImageWidth * y] = 0xff0000ff;
		}
	}



}

void TextSprite::Update()
{
	//int g_ImageWidth = 512;
	//int g_ImageHeight = 256;

	//// Update Texture
	//static DWORD g_dwCount = 0;
	//static DWORD g_dwTileColorR = 0;
	//static DWORD g_dwTileColorG = 0;
	//static DWORD g_dwTileColorB = 0;

	//const DWORD TILE_WIDTH = 16;
	//const DWORD TILE_HEIGHT = 16;

	//DWORD TILE_WIDTH_COUNT = g_ImageWidth / TILE_WIDTH;
	//DWORD TILE_HEIGHT_COUNT = g_ImageHeight / TILE_HEIGHT;

	//if (g_dwCount >= TILE_WIDTH_COUNT * TILE_HEIGHT_COUNT)
	//{
	//	g_dwCount = 0;
	//}
	//DWORD TileY = g_dwCount / TILE_WIDTH_COUNT;
	//DWORD TileX = g_dwCount % TILE_WIDTH_COUNT;

	//DWORD StartX = TileX * TILE_WIDTH;
	//DWORD StartY = TileY * TILE_HEIGHT;


	//DWORD r = g_dwTileColorR;
	//DWORD g = g_dwTileColorG;
	//DWORD b = g_dwTileColorB;


	//DWORD* pDest = (DWORD*)testptr;
	//for (DWORD y = 0; y < TILE_HEIGHT; y++)
	//{
	//	for (DWORD x = 0; x < TILE_WIDTH; x++)
	//	{
	//		if (StartX + x >= g_ImageWidth)
	//			__debugbreak();

	//		if (StartY + y >= g_ImageHeight)
	//			__debugbreak();

	//		pDest[(StartX + x) + (StartY + y) * g_ImageWidth] = 0xff000000 | (b << 16) | (g << 8) | r;
	//	}
	//}

	//g_dwCount++;
	//g_dwTileColorR += 8;
	//if (g_dwTileColorR > 255)
	//{
	//	g_dwTileColorR = 0;
	//	g_dwTileColorG += 8;
	//}
	//if (g_dwTileColorG > 255)
	//{
	//	g_dwTileColorG = 0;
	//	g_dwTileColorB += 8;
	//}
	//if (g_dwTileColorB > 255)
	//{
	//	g_dwTileColorB = 0;
	//}

	_texture->UpdateDynamicTexture(testptr);
};

void TextSprite::Render()
{

	auto& cmdList = Core::main->GetCmdList();

	TextManager::main->UpdateToSysMemory(L"helloworld", _textHandle, testptr);

	_texture->CopyCpuToGpu();

	cmdList->SetPipelineState(_shader->_pipelineState.Get());

	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(_shader->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam, sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(_shader->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}

	auto tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(1);
	Core::main->GetBufferManager()->GetTable()->CopyHandle(tableContainer.CPUHandle, _texture->GetSRVCpuHandle(), 0);
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

void TextSprite::CreateObject(int width, int height, const WCHAR* font, FontColor color, float fontsize)
{
	_textHandle =  TextManager::main->AllocTextStrcture(width, height, font, color, fontsize);
	_texture = make_shared<Texture>();
	_texture->CreateDynamicTexture(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);

	auto desc = _texture->GetResource()->GetDesc();

	_sprtieTextureParam.origintexSize = vec2(desc.Width, desc.Height);

	_sprtieTextureParam.texSamplePos.x = 0;
	_sprtieTextureParam.texSamplePos.y = 0;
	_sprtieTextureParam.texSampleSize.x = desc.Width;
	_sprtieTextureParam.texSampleSize.y = desc.Height;

	
}
