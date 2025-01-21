#include "pch.h"
#include "Sprite.h"
#include "Mesh.h"
#include "SpriteAction.h"

Sprite::Sprite()
{

}

Sprite::~Sprite()
{
	for (auto& ele : _actions)
	{
		delete ele;
	}
}

void Sprite::SetSize(vec2 size)
{

	_spriteWorldParam.ndcScale.x = size.x / WINDOW_WIDTH;
	_spriteWorldParam.ndcScale.y = size.y / WINDOW_HEIGHT;

	_ndcSize = _spriteWorldParam.ndcScale;
	_screenSize = size;
}

void Sprite::SetPos(vec3 screenPos)
{
	_spriteWorldParam.ndcPos.x = screenPos.x/ WINDOW_WIDTH;
	_spriteWorldParam.ndcPos.y = screenPos.y/ WINDOW_HEIGHT;
	_spriteWorldParam.ndcPos.z = screenPos.z;

	_ndcPos = _spriteWorldParam.ndcPos;
	_screenPos = screenPos;

}

void Sprite::SetClipingColor(vec4 color)
{
	_spriteWorldParam.clipingColor = color;
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
}

void BasicSprite::Render()
{
	auto& cmdList = Core::main->GetCmdList();

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

void BasicSprite::SetUVCoord(SpriteRect* rect)
{
	_sprtieTextureParam.texSamplePos.x = rect->left;
	_sprtieTextureParam.texSamplePos.y = rect->top;
	_sprtieTextureParam.texSampleSize.x = (rect->right - rect->left);
	_sprtieTextureParam.texSampleSize.y = (rect->bottom - rect->top);
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

	AnimationUpdate();

}

void AnimationSprite::Render()
{
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

void AnimationSprite::PushUVCoord(SpriteRect* rect)
{
	if (_texture == nullptr)
		assert(false);

	SprtieTextureParam sprtieTextureParam;

	auto desc = _texture->GetResource()->GetDesc();

	sprtieTextureParam.origintexSize = vec2(desc.Width, desc.Height);
	sprtieTextureParam.texSamplePos.x = rect->left;
	sprtieTextureParam.texSamplePos.y = rect->top;
	sprtieTextureParam.texSampleSize.x = (rect->right - rect->left);
	sprtieTextureParam.texSampleSize.y = (rect->bottom - rect->top);

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
	_currentTime += dt;  // delta time을 누적하여 진행 시간 증가

	if (_currentTime >= _frameRate) { // _frameRate에 도달하면 한 프레임을 전환
		_currentTime -= _frameRate;  // 남은 시간 처리

		_currentFrameIndex++;  // 다음 프레임으로 이동

		if (_currentFrameIndex >= _maxFrameIndex)
		{
			_currentFrameIndex = 0;  // 첫 번째 프레임으로 돌아가기
		}
	}
}
