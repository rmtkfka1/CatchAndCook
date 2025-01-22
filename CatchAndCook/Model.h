#pragma once
#include "Mesh.h"
#include "Vertex.h"

class Scene;
class Bone;
class ModelMesh;
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
	std::vector<std::shared_ptr<ModelMesh>> _modelMeshList;
	std::vector<std::shared_ptr<ModelNode>> _modelNodeList;
	std::vector<std::shared_ptr<Bone>> _modelBoneList;

	std::unordered_map<std::string, std::vector<std::shared_ptr<ModelMesh>>> _nameToMeshsTable;
	std::unordered_map<std::string, std::shared_ptr<ModelNode>> _nameToNodeTable;
	std::unordered_map<std::string, std::shared_ptr<Bone>> _nameToBoneTable;

	std::shared_ptr<ModelNode> FindNodeByName(const string& name) { return _nameToNodeTable[name]; };
	std::vector<std::shared_ptr<ModelMesh>> FindMeshsByName(const string& name) { return _nameToMeshsTable[name]; };
	std::shared_ptr<Bone> FindBoneByName(const string& name) { return _nameToBoneTable[name]; };

	void CreateGameObject(const std::shared_ptr<Scene>& scene);
	
	void Init(const wstring& path, VertexType vertexType);
	void DebugLog();

public:
	template<class T>
	static void LoadVertex(aiMesh* assimp_mesh, std::vector<T>& vertexs);
	template<>
	static void LoadVertex<Vertex_Skinned>(aiMesh* assimp_mesh, std::vector<Vertex_Skinned>& vertexs);
	template<>
	static void LoadVertex<Vertex_Static>(aiMesh* assimp_mesh, std::vector<Vertex_Static>& vertexs);

	static void LoadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs);

	void LoadBone(aiMesh* currentAIMesh, const std::shared_ptr<ModelMesh>& currentModelMesh);

private:
	int AllocBoneID() { return _boneAllocator++; };
	void AddBone(const std::shared_ptr<Bone>& bone);
	std::shared_ptr<ModelNode> AddNode(aiNode* rootNode);

private:
	int _boneAllocator = 0;
	std::shared_ptr<ModelNode> _rootNode;
};

class ModelMesh
{
	std::shared_ptr<Mesh> _mesh;
	VertexType _type = VertexType::Vertex_Static;
	std::string _name;
	int _index = -1;

public:
	std::shared_ptr<Mesh> GetMesh() { return _mesh; };
	void SetMesh(const std::shared_ptr<Mesh>& mesh) { _mesh = mesh; };
	VertexType GetType() { return _type; };
	void SetType(const VertexType& type) { _type = type; };
	void SetIndex(int index) { _index = index; };
	int GetIndex() { return _index; };
	void SetName(const std::string& name) { _name = name; };
	std::string& GetName() { return  _name; };

	std::vector<Vertex_Skinned> skinnedMeshList;
	std::vector<Vertex_Static> staticMeshList;
};

class ModelNode : public std::enable_shared_from_this<ModelNode>
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

	std::shared_ptr<GameObject> CreateGameObject(const std::shared_ptr<Scene>& scene,
	                                             const std::shared_ptr<GameObject>& parent);
};


class Bone
{
	std::string _targetNodeName;
	std::string _boneName;
	Matrix _boneTransformMatrix = Matrix::Identity;
	int _index = -1;

public:
	void SetNodeName(const std::string& name) { _targetNodeName = name; };
	std::string& GetNodeName() { return  _targetNodeName; };

	void SetName(const std::string& name) { _boneName = name; };
	std::string& GetName() { return  _boneName; };

	void SetIndex(int index) { _index = index; };
	int GetIndex() { return _index; };

	void SetTransformMatrix(const Matrix& matrix) { _boneTransformMatrix = matrix; };
	Matrix& GetTransformMatrix() { return _boneTransformMatrix; };
};
