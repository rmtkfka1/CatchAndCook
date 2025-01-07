#include "pch.h"
#include "Mesh.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "GameObject.h"
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

//[] [] []
void MeshRenderer::RenderBegin()
{
	Component::RenderBegin();
	auto& cmdList = Core::main->GetCmdList();

	for (auto& ele : _materials)  
	{
		cmdList->SetPipelineState(ele->GetShader()->_pipelineState.Get());

		ele->_container = Core::main->GetBufferManager()->GetTable()->Alloc(1);
		ele->PushMaterialData();

		GetOwner()->GetScene()->AddRenderObject(std::make_pair(ele, static_cast<RendererBase*>(this)));
	}
}

void MeshRenderer::Rendering()
{
	Component::Rendering();
}

void MeshRenderer::Rendering(RendererParam& param, const std::shared_ptr<Material>& material)
{
	auto& cmdList = Core::main->GetCmdList();
	GetOwner()->transform->PushData();
	material->PushGPUData();

	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->IASetVertexBuffers(0, 1, &_mesh->GetVertexView());
	cmdList->IASetIndexBuffer(&_mesh->GetIndexView());
	cmdList->DrawIndexedInstanced(_mesh->GetIndexCount(), 1, 0, 0, 0);
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
