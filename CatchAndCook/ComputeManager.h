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
	tableContainer _tableContainer{};

};


class Blur : public ComputeBase
{
public:
	Blur();
	virtual ~Blur();

public:
	virtual void Init();
	virtual void DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList);
	virtual void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z);
	virtual void DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList);

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
};



class ComputeManager 
{
public:
	static unique_ptr<ComputeManager> main;

public:
	void Init();
	void Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList);

private:
	shared_ptr<Blur> _blur;

};

