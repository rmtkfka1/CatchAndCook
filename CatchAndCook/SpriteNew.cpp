#include "pch.h"
#include "SpriteNew.h"

SpriteNew::SpriteNew()
{
}

SpriteNew::~SpriteNew()
{
}

void SpriteNew::Init()
{
	_firstWindowSize = vec2(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void SpriteNew::Start()
{
	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->AddCbufferSetter(static_pointer_cast<SpriteNew>(shared_from_this()));
	}
}

void SpriteNew::Update()
{
}

void SpriteNew::Update2()
{
}

void SpriteNew::Enable()
{
}

void SpriteNew::Disable()
{
}

void SpriteNew::RenderBegin()
{

}

void SpriteNew::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SpriteNew::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SpriteNew::SetDestroy()
{

}

void SpriteNew::Destroy()
{
	if (GetOwner()->GetRenderer())
	{
		GetOwner()->GetRenderer()->RemoveCbufferSetter(static_pointer_cast<SpriteNew>(shared_from_this()));
	}
}

void SpriteNew::SetData(Material* material)
{
	auto cmdList = Core::main->GetCmdList();


	// SpriteWorldParam ���� ����
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteWorldParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_spriteWorldParam, sizeof(SpriteWorldParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_WORLD_PARAM"), CbufferContainer->GPUAdress);
	}

	// SpriteTextureParam ���� ����
	{
		auto CbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::SpriteTextureParam)->Alloc(1);
		memcpy(CbufferContainer->ptr, (void*)&_sprtieTextureParam, sizeof(SprtieTextureParam));
		cmdList->SetGraphicsRootConstantBufferView(material->GetShader()->GetRegisterIndex("SPRITE_TEXTURE_PARAM"), CbufferContainer->GPUAdress);
	}
}

void SpriteNew::SetSize(vec2 size)
{
	_spriteWorldParam.ndcScale.x = size.x / _firstWindowSize.x;
	_spriteWorldParam.ndcScale.y = size.y / _firstWindowSize.y;

	_ndcSize = _spriteWorldParam.ndcScale;
	_screenSize = size;
}

void SpriteNew::SetPos(vec3 screenPos)
{
	_spriteWorldParam.ndcPos.x = screenPos.x / _firstWindowSize.x;
	_spriteWorldParam.ndcPos.y = screenPos.y / _firstWindowSize.y;
	_spriteWorldParam.ndcPos.z = screenPos.z;

	_ndcPos = _spriteWorldParam.ndcPos;
	_screenPos = screenPos;
}

void SpriteNew::SetClipingColor(vec4 color)
{
	_spriteWorldParam.clipingColor = color;
}
