#pragma once

#include "ICBufferInjector.h"
#include "Material.h"

struct TestSubMaterialParam
{
	vec2 uv;
};

CBUFFER_INJECTOR("popo", TestSubMaterialParam, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	param.uv = Vector2(source->GetPropertyVector("uv"));
)


struct DefaultMaterialParam
{
	vec2 uv;
};
CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	param.uv = Vector2(source->GetPropertyVector("uv"));
)