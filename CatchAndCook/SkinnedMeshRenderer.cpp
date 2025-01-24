#include "pch.h"
#include "SkinnedMeshRenderer.h"

#include "GameObject.h"
#include "Transform.h"


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
}

void SkinnedMeshRenderer::Start()
{
	Component::Start();
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
}

void SkinnedMeshRenderer::Rendering(const std::shared_ptr<Material>& material)
{
	auto& cmdList = Core::main->GetCmdList();

	if (material != nullptr)
		material->SetData();

	GetOwner()->_transform->SetData();

	cmdList->IASetPrimitiveTopology(_mesh->GetTopology());

	if (_mesh->GetVertexCount() != 0)
	{

		if (_mesh->GetIndexCount() != 0)
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->IASetIndexBuffer(&_mesh->GetIndexView());
			cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
		}
		else
		{
			cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
			cmdList->DrawInstanced(_mesh->GetVertexCount(), 1, 0, 0);
		}
	}
}

void SkinnedMeshRenderer::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider, other);
}


void SkinnedMeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}

void SkinnedMeshRenderer::DestroyComponentOnly()
{
	Component::DestroyComponentOnly();
}

void SkinnedMeshRenderer::SetModel(const std::shared_ptr<Model>& model)
{
	_model = model;
}

void SkinnedMeshRenderer::SetMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh = _mesh;
}

void SkinnedMeshRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_materials = _materials;
}

void SkinnedMeshRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_materials.push_back(ele);
}
