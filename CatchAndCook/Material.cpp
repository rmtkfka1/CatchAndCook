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

void Material::PushMaterialData()
{
	if (_useMaterialParams)
	{
		_cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::MateriaParam)->Alloc(1);
		memcpy(_cbufferContainer->ptr, (void*)&_params, sizeof(MaterialParams));
	}
}

void Material::SetData(tableContainer& container)
{
	SetTexture(container);
	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(4, _cbufferContainer->GPUAdress);
}

void Material::SetTexture(tableContainer& container)
{
	for (auto& [name,texture] : _propertyTextures)
	{
		Core::main->GetBufferManager()->GetTable()->CopyHandle(&container.cpuHandle, &texture->GetSRVCpuHandle(), _shader->GetRegisterIndex(name));
	}
}
