#pragma once

class Mesh;
class Texture;
class Shader;


struct SpriteWorldParam
{
	vec3 ndcPos = { 0.0f,0.0f,0.1f };
	float alpha =1.0f;

	vec2 ndcScale = { 1.0f,1.0f };
	vec2 padding = {};
};

struct SprtieTextureParam
{
	vec2 origintexSize{};
	vec2 texSamplePos{};
	vec2 texSampleSize{};
	vec2 padding;

};

struct CollisionRect
{
	float left;
	float top;
	float right;
	float bottom;
};

class Sprite 
{
public:
	Sprite();
	virtual ~Sprite();

public:
	virtual void Init() =0 ;
	virtual void Update() =0;
	virtual void Render() =0;

public:
	void SetSize(vec2 size);
	void SetPos(vec3 screenPos);
	void AddAction(std::function<void(void)> action) { _actions.push_back(action); }
	void AddCollisonMap();

protected:
	SpriteWorldParam _spriteWorldParam;

	vec3 _screenPos;
	vec3 _ndcPos;

	vec2 _screenSize;
	vec2 _ndcSize;

	vector<std::function<void(void)>> _actions;
public:
	static vector<pair<CollisionRect, Sprite*>> _collisionMap;

	friend class BasicSprite;
	friend class SpriteAction;
};

class BasicSprite : public Sprite
{

public:
	BasicSprite();
	virtual ~BasicSprite();

	
public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

	void SetUVCoord(RECT* rect);
	void SetTexture(shared_ptr<Texture> texture);

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _texture;
	SprtieTextureParam _sprtieTextureParam;
};


