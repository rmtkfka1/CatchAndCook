#pragma once
class RootSignature
{
public:

	void Init();
	ComPtr<ID3D12RootSignature>	GetGraphicsRootSignature() { return _graphicsRootSignature; }

	//temp; 
private:
	ComPtr<ID3D12RootSignature>	_graphicsRootSignature;


};

