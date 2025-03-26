#include "pch.h"
#include "Sprite.h"
#include "MeshRenderer.h"
#include "TextManager.h"

Sprite::Sprite()
{
}

Sprite::~Sprite()
{
}

void Sprite::Init()
{
	_firstWindowSize = vec2(WINDOW_WIDTH, WINDOW_HEIGHT);

}

void Sprite::Start()
{
	if (auto& renderer =GetOwner()->GetRenderer())
	{
		renderer->AddCbufferSetter(static_pointer_cast<Sprite>(shared_from_this()));
		shared_ptr<Mesh> mesh = GeoMetryHelper::LoadSprtieMesh();
		static_pointer_cast<MeshRenderer>(renderer)->AddMesh(mesh);
	}
}

void Sprite::Update()
{
	for (auto& action : _actions)
	{
		action->Execute(this);
	}
}

void Sprite::Update2()
{
}

void Sprite::Enable()
{
}

void Sprite::Disable()
{
}

void Sprite::RenderBegin()
{

}

void Sprite::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Sprite::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void Sprite::SetDestroy()
{

}

void Sprite::Destroy()
{
	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->RemoveCbufferSetter(static_pointer_cast<Sprite>(shared_from_this()));
	}
}

void Sprite::SetData(Material* material)
{
	auto cmdList = Core::main->GetCmdList();

	auto& texture = material->GetPropertyTexture("_BaseMap");

	if (texture == nullptr)
		assert(false);

	_sprtieTextureParam.origintexSize = vec2(texture->GetResource()->GetDesc().Width, texture->GetResource()->GetDesc().Height);

	if (_sprtieTextureParam.texSampleSize == vec2::Zero)
	{
		_sprtieTextureParam.texSampleSize = _sprtieTextureParam.origintexSize;
	}

	// SpriteWorldParam 
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	// SpriteTextureParam 
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam, sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}
}

void Sprite::SetSize(const vec2& size)
{
	_spriteWorldParam.ndcScale.x = size.x / _firstWindowSize.x;
	_spriteWorldParam.ndcScale.y = size.y / _firstWindowSize.y;

	_ndcSize = _spriteWorldParam.ndcScale;
	_screenSize = size;
}

void Sprite::SetPos(const vec3& screenPos)
{
	_spriteWorldParam.ndcPos.x = screenPos.x / _firstWindowSize.x;
	_spriteWorldParam.ndcPos.y = screenPos.y / _firstWindowSize.y;
	_spriteWorldParam.ndcPos.z = screenPos.z;

	_ndcPos = _spriteWorldParam.ndcPos;
	_screenPos = screenPos;
}

void Sprite::SetUVCoord(const SpriteRect& rect)
{
	_sprtieTextureParam.texSamplePos.x = rect.left;
	_sprtieTextureParam.texSamplePos.y = rect.top;
	_sprtieTextureParam.texSampleSize.x = (rect.right - rect.left);
	_sprtieTextureParam.texSampleSize.y = (rect.bottom - rect.top);
};

void Sprite::SetClipingColor(const vec4& color)
{
	_spriteWorldParam.clipingColor = color;
}

/*****************************************************************
*                                                                *
*                         TextSprite                             *
*                                                                *
******************************************************************/

TextSprite::TextSprite()
{
}

TextSprite::~TextSprite()
{
	if (_sysMemory != nullptr)
		delete[] _sysMemory;
}

void TextSprite::Init()
{
	Sprite::Init();
}

void TextSprite::Start()
{
	Sprite::Start();
}

void TextSprite::Update()
{
	if (_textChanged)
	{
		TextManager::main->UpdateToSysMemory(_text, _textHandle, _sysMemory, 4);
		_texture->UpdateDynamicTexture(_sysMemory, 4);
		_texture->CopyCpuToGpu();
		_textChanged = false;
	}

	Sprite::Update();
}

void TextSprite::Update2()
{
	Sprite::Update2();
}

void TextSprite::Enable()
{
	Sprite::Enable();
}

void TextSprite::Disable()
{
	Sprite::Disable();
}

void TextSprite::RenderBegin()
{
	Sprite::RenderBegin();

}

void TextSprite::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void TextSprite::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void TextSprite::SetDestroy()
{
	Sprite::SetDestroy();
}

void TextSprite::Destroy()
{
	Sprite::Destroy();
}

void TextSprite::SetData(Material* material)
{

	auto& cmdList = Core::main->GetCmdList();
	// SpriteWorldParam 
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	// SpriteTextureParam 
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam, sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}

	material->SetTexture("_BaseMap", _texture);

}

void TextSprite::CreateObject(int width, int height, const WCHAR* font, FontColor color, float fontsize)
{
	_textHandle = TextManager::main->AllocTextStrcture(width, height, font, color, fontsize);
	_sysMemory = new BYTE[(width * height * 4)];
	_texture = make_shared<Texture>();
	_texture->CreateDynamicTexture(DXGI_FORMAT_R8G8B8A8_UNORM, width, height);
	_sprtieTextureParam.origintexSize = vec2(width, height);

	if (color == FontColor::BLACK)
	{
		SetClipingColor(vec4(1.0f, 1.0f, 1.0f, 1.0f));

		DWORD* pDest = (DWORD*)_sysMemory;

		for (DWORD y = 0; y < height; y++)
		{
			for (DWORD x = 0; x < width; x++)
			{
				pDest[x + width * y] = 0xff'ff'ff'ff;
			}
		}
	}

	else if (color == FontColor::WHITE)
	{
		SetClipingColor(vec4(0.0f, 0.0f, 0.0f, 0.0f));

		DWORD* pDest = (DWORD*)_sysMemory;

		for (DWORD y = 0; y < height; y++)
		{
			for (DWORD x = 0; x < width; x++)
			{
				pDest[x + width * y] = 0x00'00'00'00;
			}
		}
	}
	else if (color == FontColor::CUSTOM)
	{
		SetClipingColor(vec4(0.0f, 0.0f, 0.0f, 0.0f));

		DWORD* pDest = (DWORD*)_sysMemory;
		uint32_t colorData = 0;
		colorData |= static_cast<int>(round(std::clamp(_textHandle->_customFontColor.x * 255.0, 0.0, 1.0)));
		colorData |= static_cast<int>(round(std::clamp(_textHandle->_customFontColor.y * 255.0, 0.0, 1.0))) << 8;
		colorData |= static_cast<int>(round(std::clamp(_textHandle->_customFontColor.z * 255.0, 0.0, 1.0))) << 16;
		colorData |= static_cast<int>(round(std::clamp(_textHandle->_customFontColor.w * 255.0, 0.0, 1.0))) << 24;

		for (DWORD y = 0; y < height; y++)
		{
			for (DWORD x = 0; x < width; x++)
			{
				pDest[x + width * y] = 0x00'00'00'00;
			}
		}
	}

	_sprtieTextureParam.texSamplePos.x = 0;
	_sprtieTextureParam.texSamplePos.y = 0;
	_sprtieTextureParam.texSampleSize.x = width;
	_sprtieTextureParam.texSampleSize.y = height;
}

/*****************************************************************
*                                                                *
*                         AnimationSprite                        *
*                                                                *
******************************************************************/

AnimationSprite::AnimationSprite()
{
}

AnimationSprite::~AnimationSprite()
{
}

void AnimationSprite::Init()
{
	Sprite::Init();
}

void AnimationSprite::Start()
{
	Sprite::Start();
}

void AnimationSprite::Update()
{
	Sprite::Update();

	AnimationUpdate();
}

void AnimationSprite::Update2()
{
	Sprite::Update2();
}

void AnimationSprite::Enable()
{
	Sprite::Enable();
}

void AnimationSprite::Disable()
{
	Sprite::Disable();
}

void AnimationSprite::RenderBegin()
{
	Sprite::RenderBegin();
}

void AnimationSprite::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void AnimationSprite::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void AnimationSprite::SetDestroy()
{
}

void AnimationSprite::Destroy()
{
	Sprite::Destroy();
}

void AnimationSprite::SetData(Material* material)
{
	auto cmdList = Core::main->GetCmdList();

	auto& texture = material->GetPropertyTexture("_BaseMap");

	if (texture == nullptr)
		assert(false);

	for (auto& _sprtieTextureParam : _sprtieTextureParam)
	{
		_sprtieTextureParam.origintexSize = vec2(texture->GetResource()->GetDesc().Width, texture->GetResource()->GetDesc().Height);
	}

	// SpriteWorldParam 
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	// SpriteTextureParam 
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam[_currentFrameIndex], sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}
}

void AnimationSprite::PushUVCoord(SpriteRect& rect)
{
	SprtieTextureParam sprtieTextureParam;

	sprtieTextureParam.texSamplePos.x = rect.left;
	sprtieTextureParam.texSamplePos.y = rect.top;
	sprtieTextureParam.texSampleSize.x = (rect.right - rect.left);
	sprtieTextureParam.texSampleSize.y = (rect.bottom - rect.top);

	_sprtieTextureParam.push_back(sprtieTextureParam);

	_maxFrameIndex += 1;
}

void AnimationSprite::AnimationUpdate()
{
	float dt = Time::main->GetDeltaTimeNow();
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
