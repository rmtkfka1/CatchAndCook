
#include "pch.h"
#include "Texture.h"
#include "BufferPool.h"
#include "Core.h"
D3D12_CPU_DESCRIPTOR_HANDLE Texture::_SharedDSVHandle;

Texture::Texture()
{

}

Texture::~Texture()
{
	

}

void Texture::Init(const wstring& path,TextureType type)
{
 
    wstring finalPath = _path + path;

    wstring ext = fs::path(finalPath).extension();

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(finalPath.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(finalPath.c_str(), nullptr, _image);
    else // png, jpg, jpeg, bmp
        ::LoadFromWICFile(finalPath.c_str(), WIC_FLAGS_NONE, nullptr, _image);

    HRESULT hr = ::CreateTexture(Core::main->GetDevice().Get(), _image.GetMetadata(), &_resource);

    if (FAILED(hr))
        assert(nullptr);

    vector<D3D12_SUBRESOURCE_DATA> subResources;

    hr = ::PrepareUpload(Core::main->GetDevice().Get(),
        _image.GetImages(),
        _image.GetImageCount(),
        _image.GetMetadata(),
        subResources);

    if (FAILED(hr))
        assert(nullptr);

    const uint64 bufferSize = ::GetRequiredIntermediateSize(_resource.Get(), 0, static_cast<uint32>(subResources.size()));

    ComPtr<ID3D12Resource> textureUploadHeap;
    hr = Core::main->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(textureUploadHeap.GetAddressOf()));

    if (FAILED(hr))
        assert(nullptr);

    hr = Core::main->GetDevice()->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(
            _image.GetMetadata().format,
            _image.GetMetadata().width,
            _image.GetMetadata().height,
            _image.GetMetadata().arraySize,
            _image.GetMetadata().mipLevels),
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&_resource));

    if (FAILED(hr))
        assert(nullptr);

    auto list = Core::main->GetresCmdList();
  
    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

    ::UpdateSubresources(list.Get(),
        _resource.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(subResources.size()),
        subResources.data());

    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
   
    Core::main->FlushResCMDQueue();

    Core::main->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

    srvDesc.Format = _image.GetMetadata().format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = _image.GetMetadata().mipLevels;

    _state = D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;

    switch (type)
    {
    case TextureType::Texture2D:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        break;
    case TextureType::CubeMap:
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        break;
    default:
        break;
    }

    Core::main->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
}



void Texture::ResourceBarrier(D3D12_RESOURCE_STATES after)
{
    if (_state == after)
    {
        return;
    }

    Core::main->GetCmdList()->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), _state, after));
    _state = after;
}
 

void Texture::CreateTexture(DXGI_FORMAT format, D3D12_RESOURCE_STATES initalState,uint32 width, uint32 height, TextureUsageFlags usageFlags ,bool jump,bool detphShared, vec4 clearValue )
{

    D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    desc.MipLevels = 1;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    

    if (HasFlag(usageFlags, TextureUsageFlags::RTV)) {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    }
    if (HasFlag(usageFlags, TextureUsageFlags::UAV)) {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    if (HasFlag(usageFlags, TextureUsageFlags::DSV)) {
        desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    }
     
	if (jump==false) 
    {
        D3D12_CLEAR_VALUE cvalue = {};

        if (HasFlag(usageFlags, TextureUsageFlags::RTV))
        {
            float arrFloat[4] = { clearValue.x, clearValue.y, clearValue.z, clearValue.w };
            cvalue = CD3DX12_CLEAR_VALUE(format, arrFloat);
        }

        if (HasFlag(usageFlags, TextureUsageFlags::DSV))
        {
            cvalue = CD3DX12_CLEAR_VALUE(format, 1.0f, 0);
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        }

        auto hr = Core::main->GetDevice()->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &desc,
            initalState,
            HasFlag(usageFlags, TextureUsageFlags::RTV) || HasFlag(usageFlags, TextureUsageFlags::DSV) ? &cvalue : nullptr,
            IID_PPV_ARGS(&_resource));

        if (FAILED(hr))
        {
            throw std::runtime_error("텍스쳐 생성 실패");
        }
    }

    if (HasFlag(usageFlags, TextureUsageFlags::RTV))
    {
        Core::main->GetTextureBufferPool()->AllocRTVDescriptorHandle(&_rtvHandle);
        Core::main->GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::SRV))
    {
        Core::main->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;
        Core::main->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::UAV))
    {
        Core::main->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_uavHandle);
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        Core::main->GetDevice()->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::DSV))
    {
        if (detphShared)
        {
            Core::main->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_SharedDSVHandle);
            Core::main->GetDevice()->CreateDepthStencilView(_resource.Get(), nullptr, _SharedDSVHandle);
        }
        else
        {
            Core::main->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_dsvHandle);
            Core::main->GetDevice()->CreateDepthStencilView(_resource.Get(), nullptr, _dsvHandle);
        }
    }

    _state = initalState;



}



