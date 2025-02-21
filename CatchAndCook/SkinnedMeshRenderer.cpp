#include "pch.h"
#include "SkinnedMeshRenderer.h"

#include "GameObject.h"
#include "Transform.h"
#include "Mesh.h"
#include "SkinnedHierarchy.h"


SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
}

bool SkinnedMeshRenderer::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SkinnedMeshRenderer::Init()
{
	Component::Init();
	GetOwner()->_renderer = GetCast<SkinnedMeshRenderer>();
}

void SkinnedMeshRenderer::Start()
{
	Component::Start();

	auto root = GetOwner()->GetParent();
	if (root != nullptr)
	{
		_hierarchy = root->GetComponent<SkinnedHierarchy>();
		if (_hierarchy.lock() == nullptr)
		{
			_hierarchy = root->AddComponent<SkinnedHierarchy>();
			_hierarchy.lock()->SetBoneList(_model->_modelBoneList);
			_hierarchy.lock()->SetNodeList(_model->_modelOriginalNodeList);
		}
	}
}

void SkinnedMeshRenderer::Update()
{
	Component::Update();
}

void SkinnedMeshRenderer::Update2()
{
	Component::Update2();
}

void SkinnedMeshRenderer::Enable()
{
	Component::Enable();
}

void SkinnedMeshRenderer::Disable()
{
	Component::Disable();
}

void SkinnedMeshRenderer::Destroy()
{
	Component::Destroy();
}

void SkinnedMeshRenderer::RenderBegin()
{
	Component::RenderBegin();
	
	for(int i = 0; i < _mesh.size(); i++)
	{
		auto a = GetOwner();
		auto currentMesh = _mesh[i];
		auto currentMaterial = _uniqueMaterials[i % _mesh.size()];
		currentMaterial->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
		currentMaterial->PushData();
		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(),currentMesh.get(),this);
	}

	for(int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];

		for(int i = 0; i < _mesh.size(); i++)
		{
			auto &currentMesh = _mesh[i];
			currentMaterial->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
			currentMaterial->PushData();
			SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(),currentMesh.get(),this);
		}
	}
}


void SkinnedMeshRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}


void SkinnedMeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}

void SkinnedMeshRenderer::Rendering(Material* material, Mesh* mesh)
{
	auto& cmdList = Core::main->GetCmdList();

	for(auto& data : setters) //transform , etc 
		data->SetData(material);

	if(material != nullptr)
		material->SetData();

	mesh->Redner();
}

void SkinnedMeshRenderer::DebugRendering()
{

}


void SkinnedMeshRenderer::SetModel(const std::shared_ptr<Model>& model)
{
	_model = model;
}

void SkinnedMeshRenderer::AddMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh.push_back(_mesh);
}

void SkinnedMeshRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	_uniqueMaterials = _materials;
}

void SkinnedMeshRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for(auto& ele : _materials)
		this->_uniqueMaterials.push_back(ele);
}

std::vector<std::shared_ptr<Material>>& SkinnedMeshRenderer::GetMaterials()
{
	return _uniqueMaterials;
}


void SkinnedMeshRenderer::SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_sharedMaterials = _materials;
}

void SkinnedMeshRenderer::AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for(auto& ele : _materials)
		this->_sharedMaterials.push_back(ele);
}