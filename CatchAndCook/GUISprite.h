#pragma once


struct GUISpriteParam
{
	Vector4 uvst;
};

struct GUISpriteRenderPack;

class GUISprite
{
public:
	std::shared_ptr<Texture> texture;
	std::vector<DirectX::SimpleMath::Rectangle> uvData;
	Vector2 splitWdith;

	bool isSplit = false;

	unsigned int count = 1;
	std::shared_ptr<Texture> GetTexture(int index = 0);
	std::shared_ptr<Texture> GetTextureData(int index = 0);
	void Init(const std::shared_ptr<Texture>& texture);
};


struct GUISpriteRenderPack
{
	std::shared_ptr<Texture> sprite;
	GUISpriteParam paramData;
};