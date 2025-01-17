#pragma once

class Mesh;
class Texture;
class Shader;

struct SpriteParam
{
	vec3 pos = { 0.0f,0.0f,0.1f };
	float alpha;

	vec2 scale = { 1.0f,1.0f };
	vec2 origintexSize;

	vec2 texSamplePos;
	vec2 texSampleSize;
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

public:
	int _temp = 0; //임시적으로 윈도우사이즈 맞게 변경되야해서 사용됨.

private:

	tableContainer _tableContainer;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;
	shared_ptr<Shader> _shader;
	SpriteParam _spriteParam;
	RECT _rect{ 0,0,0,0 }; //uvMapping 에 사용됨.

};

class Inventory :  public Sprite
{

public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

};

