#pragma once
class RootSignature
{
public:

	void Init();
	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }


private:
	ComPtr<ID3D12RootSignature>	_graphicsRootSignature;
	D3D12_STATIC_SAMPLER_DESC _samplerDesc{};

};

