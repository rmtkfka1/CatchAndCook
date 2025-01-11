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
void Material::SetTexture(std::string name, const std::shared_ptr<Texture>& field)
{
	if (field != nullptr)
		_propertyTextures[name] = field;

}
void Material::PushData()
{
	PushTexture();

	if (_useMaterialParams)
	{
		_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::MateriaParam)->Alloc(1);
		memcpy(_cbufferContainer->ptr, (void*)&_params, sizeof(MaterialParams));
	}

	for (auto& injector : _injectors)
		injector->Inject(GetCast<Material>());
}

void Material::SetData()
{
	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _container.gpuHandle);
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(4, _cbufferContainer->GPUAdress);

	if (_shader)
		for (auto& injector : _injectors)
			injector->SetData(_shader);
}

void Material::PushTexture()
{
	array<bool, SRV_TABLE_REGISTER_COUNT> copyCheckList;
	copyCheckList.fill(false);
	for (auto& [name,texture] : _propertyTextures)
	{
		int index = _shader->GetRegisterIndex(name);
		if (index != -1){
			copyCheckList[index] = true;
			Core::main->GetBufferManager()->GetTable()->CopyHandle(&_container.cpuHandle, &texture->GetSRVCpuHandle(), _shader->GetRegisterIndex(name));
		}
	}
	auto& tTable = _shader->GetTRegisterIndexs();
	for (auto& tIndex : tTable)
		if (tIndex < SRV_TABLE_REGISTER_COUNT && (!copyCheckList[tIndex]))
			Core::main->GetBufferManager()->GetTable()->CopyHandle(&_container.cpuHandle, 
				&ResourceManager::main->GetNoneTexture()->GetSRVCpuHandle(), tIndex);
}
