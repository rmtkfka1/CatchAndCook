#include "pch.h"
#include "SpriteNew.h"
#include "MeshRenderer.h"
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
	if (auto& renderer =GetOwner()->GetRenderer())
	{
		renderer->AddCbufferSetter(static_pointer_cast<SpriteNew>(shared_from_this()));
		shared_ptr<Mesh> mesh = GeoMetryHelper::LoadSprtieMesh();
		static_pointer_cast<MeshRenderer>(renderer)->AddMesh(mesh);
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

void SpriteNew::SetSize(const vec2& size)
{
	_spriteWorldParam.ndcScale.x = size.x / _firstWindowSize.x;
	_spriteWorldParam.ndcScale.y = size.y / _firstWindowSize.y;

	_ndcSize = _spriteWorldParam.ndcScale;
	_screenSize = size;
}

void SpriteNew::SetPos(const vec3& screenPos)
{
	_spriteWorldParam.ndcPos.x = screenPos.x / _firstWindowSize.x;
	_spriteWorldParam.ndcPos.y = screenPos.y / _firstWindowSize.y;
	_spriteWorldParam.ndcPos.z = screenPos.z;

	_ndcPos = _spriteWorldParam.ndcPos;
	_screenPos = screenPos;
}

void SpriteNew::SetUVCoord(const SpriteRect& rect)
{
	_sprtieTextureParam.texSamplePos.x = rect.left;
	_sprtieTextureParam.texSamplePos.y = rect.top;
	_sprtieTextureParam.texSampleSize.x = (rect.right - rect.left);
	_sprtieTextureParam.texSampleSize.y = (rect.bottom - rect.top);
};

void SpriteNew::SetClipingColor(const vec4& color)
{
	_spriteWorldParam.clipingColor = color;
}
