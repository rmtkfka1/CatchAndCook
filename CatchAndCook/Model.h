#pragma once
#include "Mesh.h"
#include "Vertex.h"

class ModelNode;

class AssimpPack : public std::enable_shared_from_this<AssimpPack>
{
	std::wstring name;
	std::wstring path;
	const aiScene* scene;
	std::shared_ptr<Assimp::Importer> importer;
public:
	AssimpPack();
	virtual ~AssimpPack();
	const aiScene* GetScene() const { return scene; };

	void Init(std::wstring path, bool xFlip = false);
};

class Model : public IGuid
{
public:
	std::vector<std::shared_ptr<Mesh>> _meshes;
	std::vector<std::shared_ptr<ModelNode>> _objects;

	std::unordered_map<std::string, std::vector<std::shared_ptr<Mesh>>> _nameToMeshsTable;
	std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nameToObjectTable;
	std::shared_ptr<ModelNode> FindObjectByName(const string& name) { return _nameToObjectTable[name]; };
	std::vector<std::shared_ptr<Mesh>> FindMeshsByName(const string& name) { return _nameToMeshsTable[name]; };

	template<class VertexType>
	void Init(const wstring& path)
	{
		std::shared_ptr<AssimpPack> pack = std::make_shared<AssimpPack>();
		pack->Init(path);
		const aiScene* scene = pack->GetScene();

		_meshes.reserve(scene->mNumMeshes);
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			std::shared_ptr<Mesh> currentMesh = std::make_shared<Mesh>();
			auto& currentAIMesh = scene->mMeshes[i];
			std::vector<VertexType> vertexs;
			std::vector<uint32_t> indexs;
			LoadVertex(currentAIMesh, vertexs);
			LoadIndex(currentAIMesh, indexs);
			currentMesh->Init(vertexs, indexs);
		}

		auto parentModel = std::make_shared<ModelNode>();
		auto rootNode = scene->mRootNode;
		auto rootModel = std::make_shared<ModelNode>();
		rootModel->Init(GetCast<Model>(), rootNode);
		rootModel->SetParent(parentModel);
	}

	void DebugLog();

public:
	static const aiScene* scene;

	template<class T>
	static void LoadVertex(aiMesh* assimp_mesh, std::vector<T>& vertexs);
	template<>
	static void LoadVertex<Vertex_Skinned>(aiMesh* assimp_mesh, std::vector<Vertex_Skinned>& vertexs);
	template<>
	static void LoadVertex<Vertex_Static>(aiMesh* assimp_mesh, std::vector<Vertex_Static>& vertexs);

	static void LoadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs);

};

class ModelNode : public IGuid
{
private:
	std::vector<int> _meshIndexList;

	std::weak_ptr<Model> _model;
	std::weak_ptr<ModelNode> _parent;
	std::vector<std::weak_ptr<ModelNode>> _childs;
	std::string _name;

	Matrix _localTransform = Matrix::Identity;

public:
	void SetName(const std::string& name) { _name = name; };
	std::string& GetName() { return  _name; };

	void SetLocalSRT(const Matrix& matrix) { _localTransform = matrix; };
	Matrix& GetLocalSRT() { return _localTransform; };

	void Init(shared_ptr<Model> model, aiNode* node);

	void SetParent(const std::shared_ptr<ModelNode>& object);
	void AddChild(const std::shared_ptr<ModelNode>& object);
};

class Bone
{
	std::string _targetObjectName;
public:
	void SetName(const std::string& name) { _targetObjectName = name; };
	std::string& GetName() { return  _targetObjectName; };
};