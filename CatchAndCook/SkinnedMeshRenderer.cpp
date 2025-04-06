﻿#include "pch.h"
#include "SkinnedMeshRenderer.h"

#include "GameObject.h"
#include "LightManager.h"
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

	_setter_ForwardLight = std::make_shared<ForwardLightSetter>();
	_setter_ForwardLight->Init(GetOwner().get());
}

void SkinnedMeshRenderer::Start()
{
	Component::Start();

	AddStructuredSetter(_setter_ForwardLight, BufferType::ForwardLightParam);

	auto root = GetOwner()->GetParent();
	if (root != nullptr)
	{
		auto hierarchys = GetOwner()->GetComponentsWithParents<SkinnedHierarchy>();
		if(!hierarchys.empty())
			_hierarchy = hierarchys[0];
		else
		{
			_hierarchy = root->AddComponent<SkinnedHierarchy>();
		}
		_hierarchy.lock()->SetModel(_model);
	}

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

	SetInstancing(false);
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

	RemoveStructuredSetter(_setter_ForwardLight);
}

void SkinnedMeshRenderer::RenderBegin()
{
	Component::RenderBegin();

	auto owner = GetOwner()->_transform;
	Matrix matrix;
	owner->GetLocalToWorldMatrix_BottomUp(matrix);
	BoundingBox box(Vector3(0,0,0),Vector3(5,5,5));
	box.Transform(box,matrix);
	SetBound(box);

	for(int i = 0; i < _mesh.size(); i++)
	{
		auto a = GetOwner();
		auto currentMesh = _mesh[i];
		auto currentMaterial = _uniqueMaterials[i % _uniqueMaterials.size()];

		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(),currentMesh.get(),this);
	}

	for(int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];

		for(int i = 0; i < _mesh.size(); i++)
		{
			auto &currentMesh = _mesh[i];
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

void SkinnedMeshRenderer::Rendering(Material* material, Mesh* mesh, int instanceCount)
{
	auto& cmdList = Core::main->GetCmdList();

	for(auto& data : _cbufferSetters)
		data->SetData(material);

	if(material != nullptr)
		material->SetData();

	mesh->Redner(instanceCount);
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