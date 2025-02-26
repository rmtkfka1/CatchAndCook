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
		auto shader = objects[0].material->GetShader();
		//shader->_profileInfo.GetRegisterByName()
		
		objects[0].renderer->Rendering(objects[0].material,objects[0].mesh);
	}

	_objectMap.clear();
}


void InstancingManager::AddObject(RenderObjectStrucutre & obj)
{
	InstanceID id = {obj.mesh->GetID(),obj.material->GetID()};
	_objectMap[id.id].push_back(obj);

}
