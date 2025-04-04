#include "pch.h"
#include "InstancingManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
unique_ptr<InstancingManager> InstancingManager::main;

void InstancingManager::RenderNoInstancing(RenderObjectStrucutre& RoS)
{
	auto& cmdList = Core::main->GetCmdList();
	auto& bufferManager = Core::main->GetBufferManager();
	auto& tableManager = bufferManager->GetTable();

	auto& material = RoS.material;
	auto& renderer = RoS.renderer;
	auto& mesh = RoS.mesh;
	auto& shader = material->GetShader();
	auto& structuredInfo = shader->GetTRegisterStructured();
	InstanceOffsetParam param = {};

	for (const auto& infos : structuredInfo)
	{
		const std::string& name = infos.name;
		auto& bufferType = bufferManager->GetStructuredNameToBufferType(name);
		auto& pool = bufferManager->GetStructuredBufferPool(bufferType);
		int offset = pool->GetOffset();

		auto table = tableManager->Alloc(1);

		auto& setter = RoS.renderer->FindStructuredSetter(bufferType);
		setter->SetData(pool.get());

		tableManager->CopyHandle(table.CPUHandle, pool->GetSRVHandle(), 0);

		int ROOT_OFFSET = infos.registerIndex - SRV_STRUCTURED_TABLE_REGISTER_OFFSET;
		assert(ROOT_OFFSET >= 0);
		if (ROOT_OFFSET == 1)
		{
			int a = 0;
			auto& bufferType = bufferManager->GetStructuredNameToBufferType(name);
		}
		cmdList->SetGraphicsRootDescriptorTable(SRV_STRUCTURED_TABLE_INDEX + ROOT_OFFSET, table.GPUHandle);
		param.offset[ROOT_OFFSET].x = offset;
	}

	auto cbufferContainer = bufferManager->GetBufferPool(BufferType::InstanceOffsetParam)->Alloc(1);
	memcpy(cbufferContainer->ptr, &param, sizeof(InstanceOffsetParam));
	cmdList->SetGraphicsRootConstantBufferView(4, cbufferContainer->GPUAdress);

	g_debug_draw_call++;
	renderer->Rendering(material, mesh, 1);
};

void InstancingManager::Render()
{
	auto& cmdList = Core::main->GetCmdList();
	auto& bufferManager = Core::main->GetBufferManager();
	auto& tableManager = bufferManager->GetTable();

	for (auto& [id, objects] : _objectMap)
	{
		if (objects.empty()) continue;

		auto& material = objects[0].material;
		auto& renderer = objects[0].renderer;
		auto& mesh = objects[0].mesh;
		auto& shader = material->GetShader();
		auto& structuredInfo = shader->GetTRegisterStructured();
		InstanceOffsetParam param = {};

		for (const auto& infos : structuredInfo)
		{
			const std::string& name = infos.name;
			auto& bufferType = bufferManager->GetStructuredNameToBufferType(name);
			auto& pool = bufferManager->GetStructuredBufferPool(bufferType);
			int offset = pool->GetOffset();

			auto table = tableManager->Alloc(1);

			for (auto& object : objects)
			{
				auto& setter = object.renderer->FindStructuredSetter(bufferType);
				assert(setter != nullptr);
				setter->SetData(pool.get());
			}

			tableManager->CopyHandle(table.CPUHandle, pool->GetSRVHandle(), 0);

			int ROOT_OFFSET = infos.registerIndex - SRV_STRUCTURED_TABLE_REGISTER_OFFSET;
			assert(ROOT_OFFSET >= 0);
			cmdList->SetGraphicsRootDescriptorTable(SRV_STRUCTURED_TABLE_INDEX + ROOT_OFFSET, table.GPUHandle);

			param.offset[ROOT_OFFSET].x = offset;
		}

		auto cbufferContainer = bufferManager->GetBufferPool(BufferType::InstanceOffsetParam)->Alloc(1);
		memcpy(cbufferContainer->ptr, &param, sizeof(InstanceOffsetParam));
		cmdList->SetGraphicsRootConstantBufferView(4, cbufferContainer->GPUAdress);

		g_debug_draw_call++;
		renderer->Rendering(material, mesh, objects.size());
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
