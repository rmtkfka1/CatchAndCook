#pragma once

class Mesh;
class Texture;
class Shader;
class ActionCommand;

struct SpriteWorldParam
{
	vec3 ndcPos = { 0.0f,0.0f,0.1f };
	float alpha =1.0f;

	vec2 ndcScale = { 1.0f,1.0f };
	vec2 padding = {};

	vec4 clipingColor;
};

struct SprtieTextureParam
{
	vec2 origintexSize{};
	vec2 texSamplePos{};
	vec2 texSampleSize{};
	vec2 padding;
};

struct SpriteRect
{
	float left;
	float top;
	float right;
	float bottom;
};

/*****************************************************************
*                                                                *
*                         Sprite                                 *
*                                                                *
******************************************************************/

class Sprite  : public enable_shared_from_this<Sprite>
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
	void SetClipingColor(vec4 color);  // https://imagecolorpicker.com/

	void AddAction(shared_ptr<ActionCommand> action);
	void AddChildern(shared_ptr<Sprite> child);

protected:
	SpriteWorldParam _spriteWorldParam;

	vec3 _screenPos;
	vec3 _ndcPos;

	vec2 _screenSize;
	vec2 _ndcSize;

	vec2 _firstWindowSize;

protected:
	vector<shared_ptr<Sprite>> _children;
	weak_ptr<Sprite> _parent;

public:
	friend class ActionFunc;
	bool _renderEnable = true;
	vector<shared_ptr<ActionCommand>> _actions;
};

/*****************************************************************
*                                                                *
*                         BasicSprite                            *
*                                                                *
******************************************************************/

class BasicSprite : public Sprite
{

public:
	BasicSprite();
	virtual ~BasicSprite();

public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

	void SetUVCoord(SpriteRect& rect);
	void SetTexture(shared_ptr<Texture> texture);

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _texture;
	SprtieTextureParam _sprtieTextureParam;
};


/*****************************************************************
*                                                                *
*                         TextSprite                             *
*                                                                *
******************************************************************/
enum class FontColor;
class TextHandle;

class TextSprite : public Sprite
{

public:
	TextSprite();
	virtual ~TextSprite();

public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

	void SetText(const wstring& text) { _text = text; _textChanged = true; }
	void CreateObject(int width, int height, const WCHAR* font, FontColor color, float fontsize);

	
private:
	bool _textChanged = true;
	wstring _text =L"NULL";
	shared_ptr<TextHandle> _textHandle;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _texture;
	SprtieTextureParam _sprtieTextureParam;
	BYTE* _sysMemory = nullptr;
};

/*****************************************************************
*                                                                *
*                         AnimationSprite                        *
*                                                                *
******************************************************************/

class AnimationSprite : public Sprite
{
public:

	AnimationSprite();
	virtual ~AnimationSprite();

public:
	virtual void Init();
	virtual void Update();
	virtual void Render();

	void PushUVCoord(SpriteRect& rect);
	void SetTexture(shared_ptr<Texture> texture);
	void SetFrameRate(float frameRate) { _frameRate = frameRate; }

private:
	void AnimationUpdate();

private:
	shared_ptr<Mesh> _mesh;
	shared_ptr<Shader> _shader;
	shared_ptr<Texture> _texture;
	vector<SprtieTextureParam> _sprtieTextureParam;

private:
	float _frameRate{1.0f}; //  애니메이션 진행 속도
	float _currentTime{}; //현재 애니메이션 진행 시간
	int32 _currentFrameIndex{}; // 현재 애니메이션 인덱스
	int32 _maxFrameIndex =0 ; // 최대 애니메이션 프레임
};


/*****************************************************************
*                                                                *
*                         Invetory                               *
*                                                                *
******************************************************************/

