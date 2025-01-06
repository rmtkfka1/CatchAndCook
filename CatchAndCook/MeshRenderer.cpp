#include "pch.h"
#include "Mesh.h"

#include "Material.h"
#include "MeshRenderer.h"


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
}

void MeshRenderer::Start()
{
	Component::Start();
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
}

void MeshRenderer::Rendering()
{
	Component::Rendering();

}

void MeshRenderer::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider, other);
}

void MeshRenderer::DebugRendering()
{
	Component::DebugRendering();
}

void MeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}

void MeshRenderer::DestroyComponentOnly()
{
	Component::DestroyComponentOnly();
}

void MeshRenderer::SetMesh(const std::shared_ptr<Mesh>& _mesh)
{
	this->_mesh = _mesh;
}

void MeshRenderer::SetMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_materials = _materials;
}

void MeshRenderer::AddMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_materials.push_back(ele);
}
