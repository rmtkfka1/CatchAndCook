#include "pch.h"
#include "Material.h"
#include "Core.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Texture.h"
#include "Shader.h"

uint32 Material::_instanceIDGenator =0;

Material::Material()
{
	_instanceID  = _instanceIDGenator++;
}

Material::~Material()
{

}
void Material::SetHandle(std::string name, D3D12_CPU_DESCRIPTOR_HANDLE& field)
{
	_propertyHandle[name] = field;
}

void Material::AllocTextureTable()
{
	_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
}

void Material::PushData()
{
	////temp
	//SetPropertyVector("uv", GetPropertyVector("uv") + vec4(0.01, 0, 0, 0));

	if (_shader->_profileInfo.maxTRegister >= SRV_LONG_TABLE_REGISTER_OFFSET) {
		_tableLongContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_LONG_TABLE_REGISTER_COUNT);
	}

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
	AllocTextureTable();
	PushData();
	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _tableContainer.GPUHandle);

	if (_shader->_profileInfo.maxTRegister >= SRV_LONG_TABLE_REGISTER_OFFSET) {
		Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_LONG_TABLE_INDEX, _tableLongContainer.GPUHandle);
	}

	if (_useMaterialParams)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(6, _cbufferContainer->GPUAdress);

	for (auto& injector : _shaderInjectors)
		injector->SetData(_shader);

	for (auto& injector : _customInjectors)
		injector->SetData(_shader);
}

void Material::PushHandle()
{
	array<bool, SRV_TABLE_REGISTER_COUNT> copyCheckList;
	copyCheckList.fill(false);

	auto& table =Core::main->GetBufferManager()->GetTable();

	for (auto& [name, handle] : _propertyHandle)
	{
		int index = _shader->GetRegisterIndex(name);

		if (index != -1)
		{
			if (index < SRV_TABLE_REGISTER_COUNT)
			{
				copyCheckList[index] = true;
				table->CopyHandle(_tableContainer.CPUHandle, handle, index);
			}
			else if (index >= SRV_LONG_TABLE_REGISTER_OFFSET && (index < (SRV_LONG_TABLE_REGISTER_OFFSET + SRV_LONG_TABLE_REGISTER_COUNT))) {
				table->CopyHandle(_tableLongContainer.CPUHandle, handle, index - SRV_LONG_TABLE_REGISTER_OFFSET);
			}
		}
	}

	auto& tTable = _shader->GetTRegisterIndexs();
	for (auto& tIndex : tTable)
		if (tIndex < SRV_TABLE_REGISTER_COUNT && (!copyCheckList[tIndex]))
			table->CopyHandle(_tableContainer.CPUHandle,
				ResourceManager::main->GetNoneTexture()->GetSRVCpuHandle(), tIndex);

}
