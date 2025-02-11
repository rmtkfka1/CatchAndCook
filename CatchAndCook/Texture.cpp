
#include "pch.h"
#include "Texture.h"
#include "BufferPool.h"
#include "Core.h"
#include "BufferManager.h"
D3D12_CPU_DESCRIPTOR_HANDLE Texture::_SharedDSVHandle;

Texture::Texture()
{

}

Texture::~Texture()
{
	

}

void Texture::Init(const wstring& path, TextureType type, bool relativePath)
{

    wstring finalPath = path;

    if (relativePath)
        finalPath = _path + finalPath;

    wstring ext = fs::path(finalPath).extension();

    if (ext == L".dds" || ext == L".DDS")
        ::LoadFromDDSFile(finalPath.c_str(), DDS_FLAGS_NONE, nullptr, _image);
    else if (ext == L".tga" || ext == L".TGA")
        ::LoadFromTGAFile(finalPath.c_str(), nullptr, _image);
    else if (ext == L".hdr" || ext == L".HDR")
        ::LoadFromHDRFile(path.c_str(), nullptr, _image);
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

    auto list = Core::main->GetResCmdList();
  
    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST));

    ::UpdateSubresources(list.Get(),
        _resource.Get(),
        textureUploadHeap.Get(),
        0, 0,
        static_cast<unsigned int>(subResources.size()),
        subResources.data());

    list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
   
    Core::main->FlushResCMDQueue();

    Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

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

    SetFormat(_image.GetMetadata().format);

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
 

void Texture::CreateStaticTexture(DXGI_FORMAT format, D3D12_RESOURCE_STATES initalState, uint32 width, uint32 height, TextureUsageFlags usageFlags ,bool jump, bool detphShared, vec4 clearValue )
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
     
	if (jump == false) 
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

    SetFormat(format);

    if (HasFlag(usageFlags, TextureUsageFlags::RTV))
    {
        Core::main->GetBufferManager()->GetTextureBufferPool()->AllocRTVDescriptorHandle(&_rtvHandle);
        Core::main->GetDevice()->CreateRenderTargetView(_resource.Get(), nullptr, _rtvHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::SRV))
    {
        Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);
        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MipLevels = 1;
        Core::main->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::UAV))
    {
        Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_uavHandle);
        D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
        uavDesc.Format = format;
        uavDesc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        Core::main->GetDevice()->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _uavHandle);
    }

    if (HasFlag(usageFlags, TextureUsageFlags::DSV))
    {
        if (detphShared)
        {
            Core::main->GetBufferManager()->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_SharedDSVHandle);
            Core::main->GetDevice()->CreateDepthStencilView(_resource.Get(), nullptr, _SharedDSVHandle);
        }
        else
        {
            Core::main->GetBufferManager()->GetTextureBufferPool()->AllocDSVDescriptorHandle(&_dsvHandle);
            Core::main->GetDevice()->CreateDepthStencilView(_resource.Get(), nullptr, _dsvHandle);
        }
    }

    _state = initalState;
}

void Texture::CreateDynamicTexture(DXGI_FORMAT format, uint32 width, uint32 height)
{
    D3D12_RESOURCE_DESC desc = {};
    desc.MipLevels = 1;
    desc.Format = format;
    desc.Width = width;
    desc.Height = height;
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;
    desc.DepthOrArraySize = 1;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    auto& device  = Core::main->GetDevice();
    //GPU Memory
    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &desc,
        D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
        nullptr,
        IID_PPV_ARGS(&_resource)));

    uint64 uploadBufferSize = GetRequiredIntermediateSize(_resource.Get(), 0, 1);

    ThrowIfFailed(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize),
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&_uploadResource)));

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

    device->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle);
}

void Texture::UpdateDynamicTexture(const BYTE* sysMemory)
{
    D3D12_RESOURCE_DESC Desc = _resource->GetDesc();
   
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint;
    UINT	Rows = 0;
    UINT64	RowSize = 0;
    UINT64	TotalBytes = 0;

    Core::main->GetDevice()->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

    BYTE* pMappedPtr = nullptr;
    CD3DX12_RANGE writeRange(0, 0);

    _uploadResource->Map(0, &writeRange, reinterpret_cast<void**>(&pMappedPtr));

    const BYTE* pSrc = sysMemory;
    BYTE* pDest = pMappedPtr;

    int width = Desc.Width;
    int height =Desc.Height;

    for (UINT y = 0; y < height; y++)
    {
        memcpy(pDest, pSrc, width * 4);
        pSrc += (width * 4);
        pDest += Footprint.Footprint.RowPitch;
    }
    // Unmap
    _uploadResource->Unmap(0, nullptr);

}

void Texture::CopyCpuToGpu()
{
    
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT Footprint = {};
    UINT Rows = 0;
    UINT64 RowSize = 0;
    UINT64 TotalBytes = 0;

    D3D12_RESOURCE_DESC Desc = _resource->GetDesc();
    if (Desc.MipLevels != 1) // 밉맵 레벨은 1이어야 함
        __debugbreak();

    auto& device = Core::main->GetDevice();
    auto& cmdList = Core::main->GetCmdList();

    // 서브리소스 발자국 정보 가져오기
    device->GetCopyableFootprints(&Desc, 0, 1, 0, &Footprint, &Rows, &RowSize, &TotalBytes);

    // 리소스 상태를 복사 대상으로 전환
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(), D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_COPY_DEST));

    // 텍스처 복사 위치 설정
    D3D12_TEXTURE_COPY_LOCATION destLocation = {};
    destLocation.PlacedFootprint = Footprint;
    destLocation.pResource = _resource.Get();
    destLocation.SubresourceIndex = 0;
    destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.PlacedFootprint = Footprint;
    srcLocation.pResource = _uploadResource.Get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;

    // 텍스처 복사 수행
    cmdList->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);

    // 리소스 상태를 원래 상태로 전환
    cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
        _resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
}