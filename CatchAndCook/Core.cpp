#include "pch.h"
#include "Core.h"
#include "RenderTarget.h"
#include "RootSignature.h"
#include "Texture.h"
#include "BufferPool.h"
#include "Mesh.h"
#include "Shader.h"
#include "BufferPool.h"
#include "GameObject.h"
#include "BufferManager.h"
unique_ptr<Core> Core::main=nullptr;

Core::Core()
{

}

Core::~Core()
{
    Fence();
}

void Core::Init(HWND hwnd)
{

    _hwnd = hwnd;

    InitDirectX12();

    _bufferManager = make_shared<BufferManager>();
    _bufferManager->Init();

    _renderTarget = make_shared<RenderTarget>();
    _renderTarget->Init(_swapChain);

    _rootSignature = make_shared<RootSignature>();
    _rootSignature->Init();

    _mesh = make_shared<Mesh>();

    vector<Vertex_Static> data;
  
    data.resize(4);
    //0  1
    //3  2
    data[0].position = vec3(-0.5f, 0.5f, 0.3f);
    data[1].position = vec3(0.5f, 0.5f, 0.3f);
    data[2].position = vec3(0.5f, -0.5f, 0.3f);
    data[3].position = vec3(-0.5f, -0.5f, 0.3f);

    data[0].uvs[0] = vec2(0,0);
    data[1].uvs[0] = vec2(1.0f,0);
    data[2].uvs[0] = vec2(1.0f, 1.0f);
    data[3].uvs[0] = vec2(0, 1.0f);

    data[0].uvs[1] = vec2(0, 0);
    data[1].uvs[1] = vec2(1.0f, 0);
    data[2].uvs[1] = vec2(1.0f, 1.0f);
    data[3].uvs[1] = vec2(0, 1.0f);

    vector<uint32> indices = { 0,1,2 ,0,2,3};

    _mesh->Init(data, indices);
    
    _shader = Shader::Load(L"test.hlsl",
	{
		{"PS_Main", "ps"},
		{"VS_Main", "vs"}
	});


	ShaderInfo info;
    info._zTest = false;
    info._stencilTest = false;
	_shader->SetInfo(info);
	_shader->Init(StaticProp);


    _texture = make_shared<Texture>();
    _texture->Init(L"Start.jpg");

    auto _gameObject = make_shared<GameObject>();
    _gameObject->Init();

    _gameObjects.push_back(_gameObject);
}



void Core::RenderBegin()
{
    ThrowIfFailed(_cmdMemory->Reset());
    ThrowIfFailed(_cmdList->Reset(_cmdMemory.Get(), nullptr));

    _cmdList->SetGraphicsRootSignature(_rootSignature->GetGraphicsRootSignature().Get());
    _cmdList->SetDescriptorHeaps(1, _bufferManager->GetTable()->GetDescriptorHeap().GetAddressOf());
    _renderTarget->RenderBegin();
}

void Core::Render()
{

    _cmdList->SetPipelineState(_shader->_pipelineState.Get());

    for (auto& gameObject : _gameObjects)
        gameObject->Start();
    for (auto& gameObject : _gameObjects)
        gameObject->Update();
    for (auto& gameObject : _gameObjects)
        gameObject->Update2();

    {
	    
        for (auto& gameObject : _gameObjects)
            gameObject->RenderBegin();//renderPass

        for (auto& gameObject : _gameObjects)
            gameObject->Rendering();

        auto tableContainer = _bufferManager->GetTable()->Alloc(8);
        _bufferManager->GetTable()->CopyHandle(&tableContainer.cpuHandle, &_texture->GetSRVCpuHandle(), 1);

        _cmdList->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, tableContainer.gpuHandle);
        _cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
        _cmdList->IASetIndexBuffer(&_mesh->GetIndexView());
        _cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
    }
    for (auto& gameObject : _gameObjects)
        gameObject->DebugRendering();

    for (auto& gameObject : _gameObjects)
        gameObject->Destroy();

    std::erase_if(_gameObjects, [&](const std::shared_ptr<GameObject>& gameObject) {
        return gameObject->IsDestroy();
    });

};

void Core::RenderEnd()
{

    _renderTarget->RenderEnd();


    _cmdList->Close();

    ID3D12CommandList* ppCommandLists[] = { _cmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    Fence();

    _swapChain->Present(1, 0);
    _renderTarget->ChangeIndex();

    //temp
    _bufferManager->Reset();
}





void Core::FlushResCMDQueue()
{
    _resCmdList->Close();

    ID3D12CommandList* cmdListArr[] = { _resCmdList.Get() };
    _cmdQueue->ExecuteCommandLists(_countof(cmdListArr), cmdListArr);

    Fence();

    _resCmdMemory->Reset();
    _resCmdList->Reset(_resCmdMemory.Get(), nullptr);
}




void Core::Fence()
{
    // Advance the fence value to mark commands up to this fence point.
    _fenceValue++;

    _cmdQueue->Signal(_fence.Get(), _fenceValue);

    if (_fence->GetCompletedValue() < _fenceValue)
    {
        // Fire event when GPU hits current fence.  
        _fence->SetEventOnCompletion(_fenceValue, _fenceEvent);

        // Wait until the GPU hits current fence event is fired.
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}



void Core::InitDirectX12()
{
    CreateDevice(true, true);
    CreateCmdQueue();
    CreateSwapChain();

}

void Core::CreateDevice(bool EnableDebugLayer, bool EnableGBV)
{
    ComPtr<ID3D12Debug> pDebugController = nullptr;
    ComPtr<IDXGIAdapter1> bestAdapter = nullptr;
    DXGI_ADAPTER_DESC1 bestDesc;
    ZeroMemory(&bestDesc, sizeof(bestDesc));
    size_t bestMemory = 0; // VRAM �뷮

    DWORD dwCreateFlags = 0;
    DWORD dwCreateFactoryFlags = 0;


    // ����� ���̾� Ȱ��ȭ
    if (EnableDebugLayer)
    {
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pDebugController))))
        {
            pDebugController->EnableDebugLayer();
        }


        dwCreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

        if (EnableGBV)
        {
            ID3D12Debug5* pDebugController5 = nullptr;
            if (S_OK == pDebugController->QueryInterface(IID_PPV_ARGS(&pDebugController5)))
            {
                pDebugController5->SetEnableGPUBasedValidation(TRUE);
                pDebugController5->SetEnableAutoName(TRUE);
                pDebugController5->Release();
            }
        }
    }

    // DXGI ���丮 ����
    CreateDXGIFactory2(dwCreateFactoryFlags, IID_PPV_ARGS(&_factory));

    // �����Ǵ� ��� ���� ����
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    DWORD FeatureLevelNum = _countof(featureLevels);

    // ����� ����
    for (UINT adapterIndex = 0; ; ++adapterIndex)
    {
        ComPtr<IDXGIAdapter1> pAdapter;
        HRESULT hr = _factory->EnumAdapters1(adapterIndex, &pAdapter);

        // ����Ͱ� ������ ����
        if (hr == DXGI_ERROR_NOT_FOUND)
            break;

        DXGI_ADAPTER_DESC1 AdapterDesc;
        pAdapter->GetDesc1(&AdapterDesc);

        // VRAM�� �ް�����Ʈ ������ ��ȯ
        size_t memory = AdapterDesc.DedicatedVideoMemory / (1024 * 1024);

        // ���� ���� VRAM�� ���� ����� ����
        if (memory > bestMemory)
        {
            bestMemory = memory;
            bestAdapter = pAdapter;
            bestDesc = AdapterDesc;
        }
    }



    // ���õ� ����ͷ� D3D12 ��ġ ����
    for (DWORD featerLevelIndex = 0; featerLevelIndex < FeatureLevelNum; featerLevelIndex++)
    {
        if (bestAdapter)
        {
            HRESULT hr = D3D12CreateDevice(bestAdapter.Get(), featureLevels[featerLevelIndex], IID_PPV_ARGS(&_device));

            if (SUCCEEDED(hr))
            {
                break;
            }
        }
    }

    _adapterDesc = bestDesc;

    if (pDebugController)
    {
        SetDebugLayerInfo();
    }

}

void Core::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = WINDOW_WIDTH;
    swapChainDesc.Height = WINDOW_HEIGHT;
    swapChainDesc.Format = SWAP_CHAIN_FORMAT;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SWAP_CHAIN_FRAME_COUNT;
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.Scaling = DXGI_SCALING_NONE;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc = {};
    fsSwapChainDesc.Windowed = TRUE;

    IDXGISwapChain1* pSwapChain1 = nullptr;

    if (FAILED(_factory->CreateSwapChainForHwnd(_cmdQueue.Get(), _hwnd, &swapChainDesc, &fsSwapChainDesc, nullptr, &pSwapChain1)))
    {
        __debugbreak();
    }

    pSwapChain1->QueryInterface(IID_PPV_ARGS(&_swapChain));
    pSwapChain1->Release();
    pSwapChain1 = nullptr;


}

void Core::CreateCmdQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_cmdQueue)));
    ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdMemory)));
    ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdMemory.Get(), nullptr, IID_PPV_ARGS(&_cmdList)));
    ThrowIfFailed(_cmdList->Close());


    ThrowIfFailed(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_resCmdMemory)));
    ThrowIfFailed(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _resCmdMemory.Get(), nullptr, IID_PPV_ARGS(&_resCmdList)));

    ThrowIfFailed(_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence)));
    _fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);
}

void Core::SetDebugLayerInfo()
{
    ID3D12InfoQueue* pInfoQueue = nullptr;
    _device->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
    if (pInfoQueue)
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);

        D3D12_MESSAGE_ID hide[] =
        {
            D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,
            D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,
            // Workarounds for debug layer issues on hybrid-graphics systems
            D3D12_MESSAGE_ID_EXECUTECOMMANDLISTS_WRONGSWAPCHAINBUFFERREFERENCE,
            D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
        };

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs = (UINT)_countof(hide);
        filter.DenyList.pIDList = hide;
        pInfoQueue->AddStorageFilterEntries(&filter);

        pInfoQueue->Release();
        pInfoQueue = nullptr;
    }
}





