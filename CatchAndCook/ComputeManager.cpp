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
	
#ifdef IMGUI_ON
	ImguiManager::main->_blurPtr = &_on;
#endif 


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

	_bloomTexture = make_shared<Texture>();
	_bloomTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV 
		,false, false);

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
		_Bloomshader->Init(L"bloomShader.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);
		ResourceManager::main->Add<Shader>(L"bloom", _Bloomshader);
	}
#ifdef IMGUI_ON
	ImguiManager::main->_bloomPtr = &_on;
#endif 

	
}

void Bloom::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{

}

void Bloom::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	Black(cmdList, x, y, z);

	for (int i = 0; i < _blurCount; ++i)
	{
		XBlur(cmdList, x, y, z);
		YBlur(cmdList, x, y, z);
	}

	Blooming(cmdList, x, y, z);

	DispatchEnd(cmdList);

}

void Bloom::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	_bloomTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _bloomTexture->GetResource().Get());
}

void Bloom::Resize()
{
	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_pingtexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pingtexture->GetUAVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongtexture->GetSRVCpuHandle());
	textureBufferPool->FreeSRVHandle(_pongtexture->GetUAVCpuHandle());

	textureBufferPool->FreeSRVHandle(_bloomTexture->GetUAVCpuHandle());

	_pingtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_pongtexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);

	_bloomTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		, false, false);


}

void Bloom::Black(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pingtexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_BlackShader->_pipelineState.Get());
	_tableContainer = table->Alloc(8);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pingtexture->GetUAVCpuHandle(), 4);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);
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

void Bloom::Blooming(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_Bloomshader->_pipelineState.Get());

	auto& renderTarget  = Core::main->GetRenderTarget()->GetRenderTarget();
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_pongtexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);
	_bloomTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

	_tableContainer = table->Alloc(8);

	table->CopyHandle(_tableContainer.CPUHandle, _pongtexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, renderTarget->GetSRVCpuHandle(), 1);
	table->CopyHandle(_tableContainer.CPUHandle, _bloomTexture->GetUAVCpuHandle(), 4);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

}

/*************************
*	DepthRender          *
**************************/

DepthRender::DepthRender()
{
}

DepthRender::~DepthRender()
{
}

void DepthRender::Init()
{
	_pingTexture = make_shared<Texture>();
	_pingTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV, false, false);

	_shader = make_shared<Shader>();
	ShaderInfo info;
	info._computeShader = true;
	_shader->Init(L"depthRender.hlsl", {}, ShaderArg{ {{"CS_Main","cs"}} }, info);

#ifdef IMGUI_ON
	ImguiManager::main->_depthRenderPtr = &_on;
#endif // IMGUI_ON

	
}

void DepthRender::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList, int x, int y, int z)
{
	if (_on == false)
		return;

	auto& table = Core::main->GetBufferManager()->GetTable();
	cmdList->SetPipelineState(_shader->_pipelineState.Get());
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	_tableContainer = table->Alloc(8);

	auto& depthTexture = Core::main->GetRenderTarget()->GetDSTexture();
	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE);

	table->CopyHandle(_tableContainer.CPUHandle, depthTexture->GetSRVCpuHandle(), 0);
	table->CopyHandle(_tableContainer.CPUHandle, _pingTexture->GetUAVCpuHandle(), 4);
	cmdList->SetComputeRootDescriptorTable(10, _tableContainer.GPUHandle);
	cmdList->Dispatch(x, y, z);

	auto& renderTarget = Core::main->GetRenderTarget()->GetRenderTarget();
	_pingTexture->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderTarget->ResourceBarrier(D3D12_RESOURCE_STATE_COPY_DEST);
	cmdList->CopyResource(renderTarget->GetResource().Get(), _pingTexture->GetResource().Get());

	depthTexture->ResourceBarrier(D3D12_RESOURCE_STATE_DEPTH_WRITE);
}

void DepthRender::DispatchBegin(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void DepthRender::DispatchEnd(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
}

void DepthRender::Resize()
{
	auto& textureBufferPool = Core::main->GetBufferManager()->GetTextureBufferPool();
	textureBufferPool->FreeSRVHandle(_pingTexture->GetUAVCpuHandle());


	_pingTexture->CreateStaticTexture(DXGI_FORMAT_R8G8B8A8_UNORM, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::UAV
		| TextureUsageFlags::SRV, false, false);


}



/*************************
*	ComputeManager       *
**************************/

void ComputeManager::Init()
{
	_blur = make_shared<Blur>();
	_blur->Init();

	_bloom = make_shared<Bloom>();
	_bloom->Init();

	_depthRender = make_shared<DepthRender>();
	_depthRender->Init();

}

void ComputeManager::Dispatch(ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	const int threadGroupSizeX = 16;
	const int threadGroupSizeY = 16;

	int dispatchX = static_cast<int>(std::ceil(static_cast<float>(WINDOW_WIDTH) / threadGroupSizeX));
	int dispatchY = static_cast<int>(std::ceil(static_cast<float>(WINDOW_HEIGHT) / threadGroupSizeY));

	int32 dispath[3] = {dispatchX,dispatchY,1};

	_blur->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_bloom->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

	_depthRender->Dispatch(cmdList, dispath[0], dispath[1], dispath[2]);

}

void ComputeManager::Resize()
{
	_blur->Resize();
	_bloom->Resize();
	_depthRender->Resize();
}

