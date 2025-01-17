#pragma once

class Mesh;
class Texture;
class Shader;

struct SpriteParam
{
	vec3 ndcPos = { 0.0f,0.0f,0.1f };
	float alpha =1.0f;

	vec2 ndcScale = { 1.0f,1.0f };
	vec2 origintexSize{};

	vec2 texSamplePos{};
	vec2 texSampleSize{};
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

protected:
	virtual void Init();
public:
	virtual void Update();
	virtual void Render();


public:
	void SetTexture(shared_ptr<Texture> texture, RECT* rect = nullptr);
	void SetSize(vec2 size);
	void SetPos(vec3 pos);
	void AddCollisonMap();

	vec3 _ndcPos{};
	vec2 _ndcSize{};

protected:


private:

	tableContainer _tableContainer;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;
	shared_ptr<Shader> _shader;
	SpriteParam _spriteParam;

	static vector<pair<CollisionRect, Sprite*>> _collisionMap;


};

class Inventory :  public Sprite
{

public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

};

