#include "pch.h"
#include "GUISprite.h"




void GUISprite::SetTexture(const std::shared_ptr<Texture>& texture)
{
	Vector2 size = texture->GetSize();
	Vector4 offsetSize = Vector4(0, 0, size.x, size.y);
	//uvOffsetSize = ;
}

std::shared_ptr<Texture> GUISprite::GetTexture()
{
}

std::shared_ptr<GUISprite> GUISprite::Create(const std::shared_ptr<Texture>& texture)
{
}

std::vector<std::shared_ptr<GUISprite>> GUISprite::Create(const std::shared_ptr<Texture>& texture, Vector2 gridSize)
{
}

std::vector<std::shared_ptr<GUISprite>> GUISprite::Create(const std::shared_ptr<Texture>& texture,
	std::vector<DirectX::SimpleMath::Rectangle> rects)
{
}

DirectX::SimpleMath::Rectangle GUISprite::CalculateRect(Vector2 offset, Vector2 size)
{
	return DirectX::SimpleMath::Rectangle{ static_cast<long>(offset.x), static_cast<long>(offset.y), static_cast<long>(size.x), static_cast<long>(size.y) };
}
