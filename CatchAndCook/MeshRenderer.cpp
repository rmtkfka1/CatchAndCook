#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "GameObject.h"
#include "Gizmo.h"
#include "SceneManager.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

MeshRenderer::~MeshRenderer()
{

} 

bool MeshRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void MeshRenderer::Init()
{
	Component::Init();

	GetOwner()->_renderer = GetCast<MeshRenderer>();
}

void MeshRenderer::Start()
{
	Component::Start();

	auto owner = GetOwner();
	if(owner && owner->GetType() == GameObjectType::Static)
	{
		Matrix matrix;
		owner->_transform->GetLocalToWorldMatrix_BottomUp(matrix);
		BoundingBox totalBox;
		for(int i = 0; i < _mesh.size(); i++) {
			auto currentMesh = _mesh[i];
			auto bound = currentMesh->CalculateBound(matrix);
			if(i == 0) totalBox = bound;
			else  BoundingBox::CreateMerged(totalBox,totalBox,bound);
		}
		SetBound(totalBox);
	}
}

void MeshRenderer::Update()
{
	Component::Update();

}

void MeshRenderer::Update2()
{
	Component::Update2();
}

void MeshRenderer::Enable()
{
	Component::Enable();
}

void MeshRenderer::Disable()
{
	Component::Disable();

}

void MeshRenderer::Destroy()
{
	Component::Destroy();


}

void MeshRenderer::RenderBegin()
{
	Component::RenderBegin();

	auto owner = GetOwner();
	if(owner && owner->GetType() == GameObjectType::Dynamic)
	{
		Matrix matrix;
		owner->_transform->GetLocalToWorldMatrix_BottomUp(matrix);
		BoundingBox totalBox;
		for(int i = 0; i < _mesh.size(); i++) {
			auto currentMesh = _mesh[i];
			auto bound = currentMesh->CalculateBound(matrix);
			if(i == 0) totalBox = bound;
			else  BoundingBox::CreateMerged(totalBox,totalBox,bound);
		}

		SetBound(totalBox);
	}

	if(HasGizmoFlag(Gizmo::main->_flags, GizmoFlags::Culling))
	{
		Gizmo::Width(0.2f);
		Gizmo::Box(GetBound(),vec4(0,0,1.0f,1.0f));
	}

	for (int i = 0; i < _mesh.size(); i++)
	{
		auto currentMesh = _mesh[i];
		auto currentMaterial = _uniqueMaterials[i % _uniqueMaterials.size()];

		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(), currentMesh.get(), this);
	}

	for (int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];

		for (int i = 0; i < _mesh.size(); i++)
		{
			auto &currentMesh = _mesh[i];
			SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(),currentMesh.get(),this);
		}
	}

}

void MeshRenderer::Rendering(Material* material, Mesh* mesh,int instanceCount)
{
	auto& cmdList = Core::main->GetCmdList();

	// 이 코드는 CBuffer넣는
	for (auto& data : _cbufferSetters) //transform , etc 
		data->SetData(material);

	if (material != nullptr)
		material->SetData();

	mesh->Redner(instanceCount);

}


void MeshRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);

	
}

void MeshRenderer::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void MeshRenderer::DebugRendering()
{

}

void MeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}


void MeshRenderer::AddMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh.push_back(_mesh);
}

void MeshRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_uniqueMaterials = _materials;
}

void MeshRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_uniqueMaterials.push_back(ele);
}

std::vector<std::shared_ptr<Material>>& MeshRenderer::GetMaterials()
{
	return _uniqueMaterials;
}

void MeshRenderer::SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_sharedMaterials = _materials;
}

void MeshRenderer::AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_sharedMaterials.push_back(ele);
}

