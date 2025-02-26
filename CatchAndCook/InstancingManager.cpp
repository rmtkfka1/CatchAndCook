#include "pch.h"
#include "InstancingManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
unique_ptr<InstancingManager> InstancingManager::main;

void InstancingManager::Render()
{
	//탑하나만 그렸을때 다1개씩

	for(auto& [id, objects] : _objectMap)
	{
		auto& cmdList = Core::main->GetCmdList();
		auto shader = objects[0].material->GetShader();
		auto structuredInfo = shader->GetTRegisterStructured();
		InstanceOffsetParam param = {};

		for(auto& infos : structuredInfo)
		{
			auto& name = infos.name;
			auto bufferType = Core::main->GetBufferManager()->GetStructuredNameToBufferType(name);
			auto pool = Core::main->GetBufferManager()->GetStructuredBufferPool(bufferType);
			int offset = pool->GetOffset();

			auto table = Core::main->GetBufferManager()->GetTable()->Alloc(1);

			for(auto& object : objects)
			{
				auto setter = object.renderer->FindStructuredSetter(bufferType);
				setter->SetData(pool.get());
			}

			Core::main->GetBufferManager()->GetTable()->CopyHandle(table.CPUHandle,pool->GetSRVHandle(),0);

			int ROOT_OFFSET = (infos.registerIndex - SRV_STRUCTURED_TABLE_REGISTER_OFFSET); // 이게 레지스터 위치를 0으로 옮김.
			assert(ROOT_OFFSET >= 0);
			cmdList->SetGraphicsRootDescriptorTable(SRV_STRUCTURED_TABLE_INDEX + ROOT_OFFSET,table.GPUHandle);

			param.offsets[infos.registerIndex] = offset;
		}
		auto cbufferContainer = Core::main->GetBufferManager()->GetBufferPool(BufferType::InstanceOffsetParam)->Alloc(1);
		memcpy(cbufferContainer->ptr, &param,sizeof(InstanceOffsetParam));
		cmdList->SetGraphicsRootConstantBufferView(4, cbufferContainer->GPUAdress);
		objects[0].renderer->Rendering(objects[0].material, objects[0].mesh, objects.size());
	}

	_objectMap.clear();
}

void InstancingManager::Clear()
{

}


void InstancingManager::AddObject(RenderObjectStrucutre & obj)
{
	InstanceID id = {obj.mesh->GetID(),obj.material->GetID()};
	_objectMap[id.id].push_back(obj);

}
