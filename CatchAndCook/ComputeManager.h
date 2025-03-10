#pragma once

class Texture;
class Shader;


class ComputeBase
{
public:
	ComputeBase();
	virtual ~ComputeBase();

public:
	virtual void Init()=0;
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList) = 0;
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList , int x, int y ,int z)=0;
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList) = 0;

protected:
	virtual void Resize()=0;

protected:
	tableContainer _tableContainer{};

};


class Blur : public ComputeBase
{
public:


	Blur();
	virtual ~Blur();

public:
	virtual void Init();
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
	int32 _blurCount=50;

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
	virtual void Init();
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

private:
	int32 _blurCount = 50;
	bool _on = false;
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
	float power = 32.0f;

	float g_fogMin = 0;
	float g_fogMax = 1000.0f;
	int depthRendering = 0;
	float underWaterMin = 0;

	vec3 underwaterColor =vec3(0,0.3f,0.7f);
	float underwaterMax =500.0f;
};

class DepthRender : public ComputeBase
{
	


public:
	DepthRender();
	virtual ~DepthRender();

public:
	virtual void Init();
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
	FogParam _fogParam;
	friend class ComputeManager;

};


class ComputeManager 
{
public:
	static unique_ptr<ComputeManager> main;

public:
	void Init();
	void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList);
public:
	void Resize();
private:
	shared_ptr<Blur> _blur;
	shared_ptr<Bloom> _bloom;
	shared_ptr<DepthRender> _depthRender;

};

