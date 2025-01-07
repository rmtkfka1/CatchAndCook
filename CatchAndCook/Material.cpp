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
}

void Material::SetData()
{
	Core::main->GetCmdList()->SetGraphicsRootDescriptorTable(SRV_TABLE_INDEX, _container.gpuHandle);
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(4, _cbufferContainer->GPUAdress);
}

void Material::PushTexture()
{
	for (auto& [name,texture] : _propertyTextures)
	{
		Core::main->GetBufferManager()->GetTable()->CopyHandle(&_container.cpuHandle, &texture->GetSRVCpuHandle(), _shader->GetRegisterIndex(name));
	}
}
