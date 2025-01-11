#pragma once

enum class TextureType
{
	Texture2D,
	CubeMap
};

enum class TextureUsageFlags : uint32_t
{
	None = 0,
	RTV = 1 << 0,   // Render Target View
	SRV = 1 << 1,   // Shader Resource View
	UAV = 1 << 2,    // Unordered Access View
	DSV = 1 << 3,    // Unordered Access View
};

inline TextureUsageFlags operator|(TextureUsageFlags a, TextureUsageFlags b) {
	return static_cast<TextureUsageFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline bool HasFlag(TextureUsageFlags value, TextureUsageFlags flag) {
	return (static_cast<uint32_t>(value) & static_cast<uint32_t>(flag)) != 0;
}

class Texture 
{
public:
	Texture();
	virtual ~Texture();

	void Init(const wstring& path,TextureType type = TextureType::Texture2D);
	void ResourceBarrier(D3D12_RESOURCE_STATES after);
	void CreateTexture(DXGI_FORMAT format, 
		D3D12_RESOURCE_STATES initalState ,uint32 width, uint32 height, TextureUsageFlags usageFlags , bool Jump,bool depthShared, vec4 clearValue=vec4(0,0,0,0));
	D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVCpuHandle() { return _rtvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVCpuHandle() { return _srvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetUAVCpuHandle() { return _uavHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetSharedDSVHandle() { return _SharedDSVHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVCpuHandle() { return _dsvHandle; }

	ComPtr<ID3D12Resource>& GetResource() { return _resource; }
	DXGI_FORMAT& GetFormat() { return _format; };
protected:
	DXGI_FORMAT& SetFormat(DXGI_FORMAT format) { return _format = format; };
private:
	wstring _path = L"../Resources/";
	ScratchImage			 		_image;
	ComPtr<ID3D12Resource>			_resource;
	D3D12_CPU_DESCRIPTOR_HANDLE		_rtvHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE		_srvHandle{};
	D3D12_CPU_DESCRIPTOR_HANDLE		_uavHandle{};

	static D3D12_CPU_DESCRIPTOR_HANDLE _SharedDSVHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE _dsvHandle;
	DXGI_FORMAT _format;
public:
	D3D12_RESOURCE_STATES _state; // ÃßÀû¿ë

	D3D12_GPU_VIRTUAL_ADDRESS _temp;
};

