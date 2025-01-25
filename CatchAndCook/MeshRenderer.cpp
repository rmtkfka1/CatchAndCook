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

shared_ptr<Shader> MeshRenderer::_normalDebugShader;

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

	if(_normalDebugShader == nullptr)
		_normalDebugShader = ResourceManager::main->Get<Shader>(L"normalDraw");

	GetOwner()->_renderer = GetCast<MeshRenderer>();
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

	for (int i = 0; i<_mesh.size();i++) 
	{
		auto currentMesh = _mesh[i];
		auto currentMaterial = _uniqueMaterials[i];
		currentMaterial->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
		currentMaterial->PushData();
		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial, currentMesh, static_pointer_cast<MeshRenderer>(shared_from_this()));
	}

	for (int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];
		for (int i = 0; i < _mesh.size(); i++)
		{
			auto currentMesh = _mesh[i];
			currentMaterial->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
			currentMaterial->PushData();
			SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial, currentMesh, static_pointer_cast<MeshRenderer>(shared_from_this()));
		}
	}
}

void MeshRenderer::Rendering(const std::shared_ptr<Material>& material, const std::shared_ptr<Mesh>& mesh)
{
	auto& cmdList = Core::main->GetCmdList();

	if (material != nullptr)
		material->SetData();

	for (auto& data : setters)
		data->SetData();

	cmdList->IASetPrimitiveTopology(mesh->GetTopology());

	if (mesh->GetVertexCount() != 0)
	{
		
		if (mesh->GetIndexCount() != 0)
		{
			cmdList->IASetVertexBuffers(0, 1, &mesh->GetVertexView());
			cmdList->IASetIndexBuffer(&mesh->GetIndexView());
			cmdList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
		}
		else
		{
			cmdList->IASetVertexBuffers(0, 1, &mesh->GetVertexView());
			cmdList->DrawInstanced(mesh->GetVertexCount(), 1, 0, 0);
		}
	}

	setters.clear();
}




void MeshRenderer::Collision(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::Collision(collider, other);
}

void MeshRenderer::DebugRendering()
{
	//auto& cmdList = Core::main->GetCmdList();

	//for (auto& mesh : _mesh)
	//{
	//	if (_drawNormal)
	//	{
	//		cmdList->SetPipelineState(_normalDebugShader->_pipelineState.Get());

	//		GetOwner()->_transform->SetData();

	//		cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	//		if (mesh->GetVertexCount() != 0)
	//		{
	//			if (mesh->GetIndexCount() != 0)
	//			{
	//				cmdList->IASetVertexBuffers(0, 1, &mesh->GetVertexView());
	//				cmdList->IASetIndexBuffer(&mesh->GetIndexView());
	//				cmdList->DrawIndexedInstanced(mesh->GetIndexCount(), 1, 0, 0, 0);
	//			}

	//			else
	//			{
	//				cmdList->IASetVertexBuffers(0, 1, &mesh->GetVertexView());
	//				cmdList->DrawInstanced(mesh->GetVertexCount(), 1, 0, 0);
	//			}
	//		}

	//	}
	//}

}

void MeshRenderer::SetDestroy()
{
	Component::SetDestroy();
}

void MeshRenderer::DestroyComponentOnly()
{
	Component::DestroyComponentOnly();
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

void MeshRenderer::SetSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	this->_sharedMaterials = _materials;
}

void MeshRenderer::AddSharedMaterials(const std::vector<std::shared_ptr<Material>>& _materials)
{
	for (auto& ele : _materials)
		this->_sharedMaterials.push_back(ele);
}

