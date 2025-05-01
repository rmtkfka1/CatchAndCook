#pragma once

class Texture;
class Shader;


class ComputeBase
{
public:
	ComputeBase();
	virtual ~ComputeBase();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture) = 0;
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList) = 0;
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList , int x, int y ,int z)=0;
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList) = 0;

protected:
	virtual void Resize()=0;

protected:
	TableContainer _tableContainer{};

};


class Blur : public ComputeBase
{
public:


	Blur();
	virtual ~Blur();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();
private:
	void XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x ,int y ,int z);

private:
	int32 _blurCount = 6;

	bool _on =false;

	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;
	shared_ptr<Shader> _XBlurshader;
	shared_ptr<Shader> _YBlurshader;

	friend class ComputeManager;
};

class Bloom : public ComputeBase
{

public:
	Bloom();
	virtual ~Bloom();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize(); 
private:
	void Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void Blooming(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

	bool _on = true;

private:
	int32 _blurCount = 10;
	shared_ptr<Texture> _bloomTexture;
	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;

	shared_ptr<Shader> _XBlurshader;
	shared_ptr<Shader> _YBlurshader;
	shared_ptr<Shader> _BlackShader;
	shared_ptr<Shader> _Bloomshader;
	friend class ComputeManager;
};

class FieldBloom : public ComputeBase
{

public:
	FieldBloom();
	virtual ~FieldBloom();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();
private:
	void Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	void Blooming(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

	bool _on = true;

private:
	int32 _blurCount = 10;
	shared_ptr<Texture> _bloomTexture;
	shared_ptr<Texture> _pingtexture;
	shared_ptr<Texture> _pongtexture;

	shared_ptr<Shader> _XBlurshader;
	shared_ptr<Shader> _YBlurshader;
	shared_ptr<Shader> _BlackShader;
	shared_ptr<Shader> _Bloomshader;
	friend class ComputeManager;
};


struct FogParam
{
	

	vec3 g_fogColor = vec3(1, 1, 1);
	float power = 1.0f;

	float g_fogMin = 0;
	float g_fogMax = 1000.0f;
	int depthRendering = 0;
	float padding = 0;

};

class DepthRender : public ComputeBase
{
	


public:
	DepthRender();
	virtual ~DepthRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;
	FogParam _fogParam;

	friend class ComputeManager;
};

class FieldFogRender : public ComputeBase
{

public:
	FieldFogRender();
	virtual ~FieldFogRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;
	FogParam _fogParam;

	bool _onOff = true;

	friend class ComputeManager;
};

struct UnderWaterParam
{
	vec3 g_fogColor = vec3(0.072f, 0.830f, 0.714f);
	float g_fog_power = 1.0f;

	vec3 g_underWaterColor = vec3(0.5f, 0.6f, 0.6f);
	float g_fogMin = 0;

	vec2 padding;
	int g_on = -1;
	float g_fogMax = 1000.0f;
};

class UnderWaterEffect : public ComputeBase
{

public:
	UnderWaterEffect();
	virtual ~UnderWaterEffect();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Texture> _colorGrading;
	shared_ptr<Shader> _shader;

	UnderWaterParam	_underWaterParam;

	friend class ComputeManager;

};

struct VignetteParam
{
	float power = 1;
	Vector3 color = Vector3(0.1,0.04,0.08);
	float startRound = 0.85;
	float Range = 1.4;
	Vector2 padding;

};
class VignetteRender : public ComputeBase
{

public:
	VignetteRender();
	virtual ~VignetteRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;

	VignetteParam _vignetteParam;
	bool _onOff = true;

	friend class ComputeManager;
};

class SSAORender : public ComputeBase
{

public:
	SSAORender();
	virtual ~SSAORender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();


public:
	std::shared_ptr<Texture> _ssaoTexture;

private:
	shared_ptr<Texture> _pingTexture;
	shared_ptr<Shader> _shader;

	bool ssaoOnOff = true;

	friend class ComputeManager;
};

class ColorGradingRender : public ComputeBase
{

public:
	ColorGradingRender();
	virtual ~ColorGradingRender();

public:
	virtual void Init(shared_ptr<Texture>& pingTexture, shared_ptr<Texture>& pongTexture);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);

private:
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	virtual void Resize();

private:
	shared_ptr<Texture> _pingTexture;
	std::shared_ptr<Texture> _ssaoTexture;
	shared_ptr<Shader> _shader;

	bool colorGradingOnOff = true;

	friend class ComputeManager;
};



class ComputeManager 
{
public:
	static unique_ptr<ComputeManager> main;

public:
	void Init();
	void DispatchAfterDeferred(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void DispatchMainField(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList);
public:
	void Resize();
public:

	shared_ptr<Texture> _pingTexture;
	shared_ptr<Texture> _pongTexture;

	shared_ptr<Blur> _blur;
	shared_ptr<Bloom> _bloom;
	shared_ptr<FieldBloom> _fieldBloom;
	shared_ptr<DepthRender> _depthRender;
	shared_ptr<UnderWaterEffect> _underWaterEffect;

	shared_ptr<VignetteRender> _vignetteRender;
	shared_ptr<SSAORender> _ssaoRender;
	shared_ptr<FieldFogRender> _fieldFogRender;
	shared_ptr<ColorGradingRender> _colorGradingRender;
	// color grading
};

