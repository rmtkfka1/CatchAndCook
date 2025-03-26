#include "pch.h"
#include "Sprite.h"
#include "MeshRenderer.h"

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


