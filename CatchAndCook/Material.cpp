#include "pch.h"
#include "Material.h"
#include "Core.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Texture.h"
#include "Shader.h"

Material::~Material()
{

}
void Material::SetHandle(std::string name, D3D12_CPU_DESCRIPTOR_HANDLE& field)
{
	_propertyHandle[name] = field;
}

void Material::PushData()
{
	////temp
	//SetPropertyVector("uv", GetPropertyVector("uv") + vec4(0.01, 0, 0, 0));

	PushHandle();

	if (_useMaterialParams)
	{
		_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::MateriaParam)->Alloc(1);
		memcpy(_cbufferContainer->ptr, (void*)&_params, sizeof(MaterialParams));
	}

	for (auto& injector : _shaderInjectors)
		injector->Inject(GetCast<Material>());
	for (auto& injector : _customInjectors)
		injector->Inject(GetCast<Material>());
}

void Material::SetData()
{
	//텍스쳐바인딩
	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _tableContainer.GPUHandle);

	if(_useMaterialParams)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(4, _cbufferContainer->GPUAdress);

	for(auto& injector : _shaderInjectors)
		injector->SetData(_shader);
	for(auto& injector : _customInjectors)
		injector->SetData(_shader);
}

void Material::PushHandle()
{
	array<bool, SRV_TABLE_REGISTER_COUNT> copyCheckList;
	copyCheckList.fill(false);

	for (auto& [name,handle] : _propertyHandle)
	{
		int index = _shader->GetRegisterIndex(name);

		if (index != -1)
		{
			copyCheckList[index] = true;
			Core::main->GetBufferManager()->GetTable()->CopyHandle(_tableContainer.CPUHandle, handle, index);
		}
	}

	auto& tTable = _shader->GetTRegisterIndexs();
	for (auto& tIndex : tTable)
		if (tIndex < SRV_TABLE_REGISTER_COUNT && (!copyCheckList[tIndex]))
			Core::main->GetBufferManager()->GetTable()->CopyHandle(_tableContainer.CPUHandle, 
				ResourceManager::main->GetNoneTexture()->GetSRVCpuHandle(), tIndex);

}
