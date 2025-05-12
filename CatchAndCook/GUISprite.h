#pragma once


struct GUISpriteParam
{
	//x,y offset
	//z,w size
	Vector4 uvOffsetSize;
};

struct GUISpriteRenderPack;

class GUISprite
{
public:
	std::shared_ptr<Texture> texture;
	DirectX::SimpleMath::Rectangle uvOffsetSize;

	void SetTexture(const std::shared_ptr<Texture>& texture);
	std::shared_ptr<Texture> GetTexture();
	std::shared_ptr<GUISprite> Create(const std::shared_ptr<Texture>& texture);
	std::vector<std::shared_ptr<GUISprite>> Create(const std::shared_ptr<Texture>& texture, Vector2 gridSize);
	std::vector<std::shared_ptr<GUISprite>> Create(const std::shared_ptr<Texture>& texture, std::vector<DirectX::SimpleMath::Rectangle> rects);

	DirectX::SimpleMath::Rectangle CalculateRect(Vector2 offset, Vector2 size);
};