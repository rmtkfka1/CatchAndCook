#include "pch.h"
#include "ModelNode.h"

#include "GameObject.h"
#include "ModelMesh.h"
#include "Transform.h"
#include "Component.h"
#include "MeshRenderer.h"


void ModelNode::Init(shared_ptr<Model> model, aiNode* node)
{
	_model = model;
	std::string name = convert_assimp::Format(node->mName);
	SetName(name);
	SetLocalSRT(convert_assimp::Format(node->mTransformation));

	_meshIndexList.reserve(node->mNumMeshes);
	for (int i = 0; i < node->mNumMeshes; i++)
		_meshIndexList.push_back(node->mMeshes[i]);
	std::vector<std::shared_ptr<ModelMesh>> meshList;
	for (auto& meshIndex : _meshIndexList)
		meshList.push_back(model->_modelMeshList[meshIndex]);

	model->_nameToNodeTable[name] = shared_from_this();
	model->_nameToMeshsTable[name] = meshList;

}

void ModelNode::SetParent(const std::shared_ptr<ModelNode>& object)
{
	if (object != nullptr)
	{
		_parent = object;
		object->AddChild(shared_from_this());
	}
}

void ModelNode::AddChild(const std::shared_ptr<ModelNode>& object)
{
	_childs.push_back(object);
}

std::shared_ptr<GameObject> ModelNode::CreateGameObject(const std::shared_ptr<Scene>& scene,
	const std::shared_ptr<GameObject>& parent)
{
	auto currentGameObject = scene->CreateGameObject(std::to_wstring(GetName()));
	currentGameObject->transform->SetLocalSRTMatrix(_localTransform);
	currentGameObject->SetParent(parent);

	for (auto& meshIndex : _meshIndexList)
	{
		auto meshRenderer = currentGameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetMesh(_model.lock()->FindMeshByIndex(meshIndex)->GetMesh());
		std::shared_ptr<Material> material = std::make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"DefaultForward"));
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", ResourceManager::main->Get<Texture>(L"None_Debug"));
		meshRenderer->AddMaterials({ material });
	}

	for (auto& child : _childs)
		child.lock()->CreateGameObject(scene, currentGameObject);

	return currentGameObject;
}