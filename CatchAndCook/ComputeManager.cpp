#include "pch.h"
#include "ComputeManager.h"
#include "Shader.h"
#include "Texture.h"
unique_ptr<ComputeManager> ComputeManager::main = nullptr;

/*************************
*    ComputeBase         *
**************************/

ComputeBase::ComputeBase()
{
}

ComputeBase::~ComputeBase()
{
}


/*************************
*			Blur         *
**************************/

Blur::Blur()
{
}

Blur::~Blur()
{
}

void Blur::Init()
{
	_pingtexture = make_shared<Texture>();
	_pingtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_pongtexture = make_shared<Texture>();
	_pongtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);


	{
		_XBlurshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;

		
		_XBlurshader->Init(L"xblur.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}}}, info);
		ResourceManager::main->Add<Shader>(L"xblur", _XBlurshader);
	}
	
	{
		_YBlurshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;

		_YBlurshader->Init(L"yblur.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"yblur", _YBlurshader);
	}

	ImguiManager::main->_blurPtr = &_on;
};

void Blur::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();

	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);

	cmdList->CopyResource(_pingtexture->GetResource().Get(), intermediateTexutre->GetResource().Get());
}

void Blur::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	DispatchBegin(cmdList);

	for (int i = 0; i < _blurCount; ++i)
	{
		XBlur(cmdList, x, y, z);
		YBlur(cmdList, x, y, z);
	}

	DispatchEnd(cmdList);
}

void Blur::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(intermediateTexutre->GetResource().Get(), _pongtexture->GetResource().Get());
}

void Blur::XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList,int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_XBlurshader->_pipelineState.Get());

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	_tableContainer = table->Alloc(8);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetUAVCpuHandle(), 4);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Blur::YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_YBlurshader->_pipelineState.Get());

	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	_tableContainer = table->Alloc(8);

	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 4);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);

	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Blur::Resize()
{
	auto& textureBufferPool =Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_pingtexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pingtexture->GetUAVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongtexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongtexture->GetUAVCpuHandle());

	_pingtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_pongtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);
}

/*************************
*			Bloom        *
**************************/
Bloom::Bloom()
{
}

Bloom::~Bloom()
{
}

void Bloom::Init()
{
	_pingtexture = make_shared<Texture>();
	_pingtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_pongtexture = make_shared<Texture>();
	_pongtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_XBlurshader = ResourceManager::main->Get<Shader>(L"xblur");
	_YBlurshader = ResourceManager::main->Get<Shader>(L"yblur");

	{
		_BlackShader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_BlackShader->Init(L"blackShader.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"blackShader", _BlackShader);
	}

	{
		_Bloomshader = make_shared<Shader>();
		ShaderInfo info;
		info._computeShader = true;
		_BlackShader->Init(L"bloom.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"bloom", _BlackShader);
	}
	
}

void Bloom::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();
	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(_pingtexture->GetResource().Get(), intermediateTexutre->GetResource().Get());
}

void Bloom::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	DispatchBegin(cmdList);

	for (int i = 0; i < _blurCount; ++i)
	{
		XBlur(cmdList, x, y, z);
		YBlur(cmdList, x, y, z);
	}


	DispatchEnd(cmdList);
}

void Bloom::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& intermediateTexutre = Core::main->GetRenderTarget()->GetRenderTarget();
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	intermediateTexutre->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(intermediateTexutre->GetResource().Get(), _pongtexture->GetResource().Get());
}

void Bloom::Resize()
{
	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_pingtexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pingtexture->GetUAVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongtexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongtexture->GetUAVCpuHandle());

	_pingtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_pongtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);
}

void Bloom::XBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_XBlurshader->_pipelineState.Get());
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(8);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetUAVCpuHandle(), 4);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}

void Bloom::YBlur(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_YBlurshader->_pipelineState.Get());
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_tableContainer = table->Alloc(8);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 4);
	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
}


/*************************
*	ComputeManager       *
**************************/

void ComputeManager::Init()
{
	_blur = make_shared<Blur>();
	_blur->Init();

	//_bloom = make_shared<Bloom>();
	//_bloom->Init();
}

void ComputeManager::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	const int threadGroupSizeX = 16;
	const int threadGroupSizeY = 16;

	int dispatchX = static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / threadGroupSizeX));
	int dispatchY = static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / threadGroupSizeY));

	int32 dispath[3] = {dispatchX,dispatchY,1};

	_blur->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	/*_bloom->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);*/

}

void ComputeManager::Resize()
{
	_blur->Resize();
	_bloom->Resize();
}

