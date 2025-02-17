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

	if(HasInstance())
		_instanceBuffer->Free();
}

void MeshRenderer::RenderBegin()
{
	Component::RenderBegin();

	for (int i = 0; i < _mesh.size(); i++)
	{
		auto currentMesh = _mesh[i];
		auto currentMaterial = _uniqueMaterials[i % _mesh.size()];
		currentMaterial->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
		currentMaterial->PushData();
		SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(), currentMesh.get(), this);
	}

	for (int j = 0; j < _sharedMaterials.size(); j++)
	{
		auto currentMaterial = _sharedMaterials[j];

		for (int i = 0; i < _mesh.size(); i++)
		{
			auto &currentMesh = _mesh[i];
			currentMaterial->_tableContainer = Core::main->GetBufferManager()->GetTable()->Alloc(SRV_TABLE_REGISTER_COUNT);
			currentMaterial->PushData();
			SceneManager::main->GetCurrentScene()->AddRenderer(currentMaterial.get(), currentMesh.get(), this);
		}
	}

}

void MeshRenderer::Rendering(Material* material, Mesh* mesh)
{
	auto& cmdList = Core::main->GetCmdList();

	for (auto& data : setters) //transform , etc 
		data->SetData(material);

	if (material != nullptr)
		material->SetData();

	if(HasInstance())
	{
		Core::main->GetCmdList()->IASetVertexBuffers(1,1,&_instanceBuffer->_bufferView);
		mesh->Redner(_instanceBuffer->writeIndex);
	}
	else
	{
		mesh->Redner();
	}

};

void MeshRenderer::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void MeshRenderer::DebugRendering()
{
	/*auto& cmdList = Core::main->GetCmdList();

	for (auto& mesh : _mesh)
	{
		if (_normalDebugShader)
		{
			cmdList->SetPipelineState(_normalDebugShader->_pipelineState.Get());

			GetOwner()->_transform->SetData();

			cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

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

		}
	}*/
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

void MeshRenderer::SetInstance(InstanceBufferContainer* _instance)
{
	_instanceBuffer = _instance;
	_hasInstance = _instanceBuffer != nullptr;
}

