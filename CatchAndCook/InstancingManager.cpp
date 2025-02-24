#include "pch.h"
#include "InstancingManager.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
unique_ptr<InstancingManager> InstancingManager::main;

void InstancingManager::Render()
{
	//탑하나만 그렸을때 다1개씩

	for(auto& [id,objects] : _objectMap)
	{
		for(auto& ele :objects)
		{
			Matrix mat;
			ele.object->_transform->GetLocalToWorldMatrix_BottomUp(mat);

			Instance_Transform data;
			data.localToWorld = mat;
			data.worldToLocal = mat.Invert();
			AddParam(id,data);
		}
		
		objects[0].renderer->Rendering(objects[0].material,objects[0].mesh, _bufferMap[id]);
	}

	_objectMap.clear();
}

void InstancingManager::Clear()
{
	for(auto& ele :_bufferMap)
	{
		ele.second->Clear();
	}
}

void InstancingManager::AddObject(RenderObjectStrucutre & obj)
{
	InstanceID id = {obj.mesh->GetID(),obj.material->GetID()};
	_objectMap[id.id].push_back(obj);

}

void InstancingManager::AddParam(uint64 instanceID,Instance_Transform& data)
{
	if(_bufferMap.find(instanceID) == _bufferMap.end())
	{
		_bufferMap[instanceID] = make_shared<StructuredBuffer<Instance_Transform>>();
		_bufferMap[instanceID]->Init(500);
	}

	_bufferMap[instanceID]->AddData(data);
}

