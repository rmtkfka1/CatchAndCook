#include "pch.h"
#include "RootSignature.h"
#include "BufferPool.h"
#include "Core.h"



void RootSignature::Init()
{
	// 정적 샘플러 설정
	D3D12_STATIC_SAMPLER_DESC samplerDesc[6] = {};
	samplerDesc[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[0].MipLODBias = 0;
	samplerDesc[0].MaxAnisotropy = 0;
	samplerDesc[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[0].MinLOD = 0.0f;
	samplerDesc[0].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[0].ShaderRegister = 0;
	samplerDesc[0].RegisterSpace = 0;
	samplerDesc[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	samplerDesc[1].Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc[1].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	samplerDesc[1].MipLODBias = 0;
	samplerDesc[1].MaxAnisotropy = 0;
	samplerDesc[1].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[1].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[1].MinLOD = 0.0f;
	samplerDesc[1].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[1].ShaderRegister = 1;
	samplerDesc[1].RegisterSpace = 0;
	samplerDesc[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	samplerDesc[2].Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc[2].MinLOD = 0.0f;
	samplerDesc[2].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[2].MipLODBias = 0;
	samplerDesc[2].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[2].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[2].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc[2].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[2].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[2].MaxAnisotropy = 4;
	samplerDesc[2].RegisterSpace = 0;
	samplerDesc[2].ShaderRegister = 2;

	samplerDesc[3].Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc[3].MinLOD = 0.0f;
	samplerDesc[3].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[3].MipLODBias = 0;
	samplerDesc[3].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[3].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[3].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc[3].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[3].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[3].MaxAnisotropy = 8;
	samplerDesc[3].RegisterSpace = 0;
	samplerDesc[3].ShaderRegister = 3;

	samplerDesc[4].Filter = D3D12_FILTER_ANISOTROPIC;
	samplerDesc[4].MinLOD = 0.0f;
	samplerDesc[4].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[4].MipLODBias = 0;
	samplerDesc[4].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[4].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[4].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	samplerDesc[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc[4].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	samplerDesc[4].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	samplerDesc[4].MaxAnisotropy = 16;
	samplerDesc[4].RegisterSpace = 0;
	samplerDesc[4].ShaderRegister = 4;

	samplerDesc[5].Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc[5].MinLOD = 0.0f;
	samplerDesc[5].MaxLOD = D3D12_FLOAT32_MAX;
	samplerDesc[5].MipLODBias = 0;
	samplerDesc[5].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[5].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[5].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
	samplerDesc[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	samplerDesc[5].ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	samplerDesc[5].BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
	samplerDesc[5].MaxAnisotropy = 1;
	samplerDesc[5].RegisterSpace = 0;
	samplerDesc[5].ShaderRegister = 5;


	auto G_SHADOW_SRV_RANGE   = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1,20);
	auto G_POSITION_SRV_RANGE = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 21);
	auto G_NORAML_SRV_RANGE  = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 22);
	auto G_COLOR_SRV_RANGE   = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 23);
	auto G_DEPTH_SRV_RANGE   = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 24);
	auto SRV_Range = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, SRV_TABLE_REGISTER_COUNT, 0);
	auto SRV2_Range = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,SRV_LONG_TABLE_REGISTER_COUNT, SRV_LONG_TABLE_REGISTER_OFFSET);
	auto UAV_Range = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, UAV_TABLE_REGISTER_COUNT, 0);
	auto SPRITE_RANGE = CD3DX12_DESCRIPTOR_RANGE(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 18);

	array<CD3DX12_ROOT_PARAMETER, 21> param;

	for (int cbv_index=0; cbv_index < CBV_ROOT_INDEX_COUNT; cbv_index++)
		param[cbv_index].InitAsConstantBufferView(cbv_index); 

	param[SRV_TABLE_INDEX].InitAsDescriptorTable(1, &SRV_Range);
	param[SRV_LONG_TABLE_INDEX].InitAsDescriptorTable(1,&SRV2_Range);
	param[UAV_TABLE_INDEX].InitAsDescriptorTable(1, &UAV_Range);
	param[SPRITE_TABLE_INDEX].InitAsDescriptorTable(1, &SPRITE_RANGE);

	param[GLOBAL_SRV_SHADOW_INDEX].InitAsDescriptorTable(1, &G_SHADOW_SRV_RANGE);
	param[GLOBAL_SRV_POSITION_INDEX].InitAsDescriptorTable(1, &G_POSITION_SRV_RANGE);
	param[GLOBAL_SRV_NORAML_INDEX].InitAsDescriptorTable(1, &G_NORAML_SRV_RANGE);
	param[GLOBAL_SRV_COLOR_INDEX].InitAsDescriptorTable(1, &G_COLOR_SRV_RANGE);
	param[GLOBAL_SRV_DEPTH_INDEX].InitAsDescriptorTable(1, &G_DEPTH_SRV_RANGE);
	
	// 루트 서명 설정
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.NumParameters = param.size();
	rootSignatureDesc.pParameters = param.data();
	rootSignatureDesc.NumStaticSamplers = 6;
	rootSignatureDesc.pStaticSamplers = samplerDesc;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		| D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;

	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	Core::main->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&_graphicsRootSignature));

}

