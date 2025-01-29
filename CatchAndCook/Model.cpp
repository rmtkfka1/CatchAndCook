#include "pch.h"
#include "Model.h"
#include "Bone.h"
#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "Mesh.h"
#include "ModelMesh.h"
#include "ModelNode.h"
#include "Transform.h"
#include "Mesh.h"
#include "Vertex.h"

AssimpPack::AssimpPack()
{
}

AssimpPack::~AssimpPack()
{
}

void AssimpPack::Init(std::wstring path, bool xFlip)
{
	auto p = std::filesystem::path(path);
	assert(std::filesystem::exists(p));

    importer = std::make_shared<Assimp::Importer>();

    unsigned int flag = //aiProcess_MakeLeftHanded | // �޼� ��ǥ��� ����
        //aiProcess_ConvertToLeftHanded |
        aiProcess_FlipWindingOrder | //CW, CCW �ٲٴ°���.
        aiProcess_FlipUVs | // ���״�� uv�� y���� ������. �׸��� bitangent�� ������.
        aiProcess_Triangulate | // 4���� 5������ 3��������
        //aiProcess_GenSmoothNormals | // Normal�� ������ Smmoth Normal ����
        aiProcess_GenNormals | // Normal�� ������ Normal ����
        //aiProcess_ImproveCacheLocality | // �ﰢ�� ����. �� �Ǹ� �Ѻ��� ĳ����Ʈ���� ���� �ﰢ�� ��������.
        //aiProcess_GenUVCoords | // UV������ UV ����ϰ�[ ��Ű��
        aiProcess_CalcTangentSpace | // ź��Ʈ ���
        //aiProcess_SplitLargeMeshes |// �Ž��� �ʹ� Ŭ�� �ɰ��°� �Ž� Ŭ�� ������ ����.
        //aiProcess_Debone | �սǾ��� �� ����. �� ��������.
        //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components ����
        //aiProcess_PreTransformVertices | // root Node�� ������ ��� ���� ���� ���� ��źȭ. ���� ����.
        //aiProcess_ValidateDataStructure | // ���� ��ȿ�� �˻�
        //aiProcess_RemoveRedundantMaterials | // �ߺ��̳� �Ⱦ��°� ����
        //aiProcess_FixInfacingNormals | //�߸� ����Ǽ� ���峭 ��� ���� ����
        //aiProcess_FindDegenerates | //�ﰢ������ ���� ���Ĺ����� �����̳� ���� �ǹ����µ�, �̰� Line�̳� Point�� ��ȯ�ϴ°���. �Ⱦ��°� ����.
        //aiProcess_FindInvalidData | //��ȿ���� �ʴ� ������ ������ ����, ���� UV�� ������. �̷��� �����ϰ� ���� aiProcess_GenNormals������ ���Ӱ� �������ٰ���. �ִϸ��̼ǿ����� ������ �ִٰ���.
        //aiProcess_GenUVCoords  | //UV�� ��ü������ �����. �𵨸������� �����ϴ°� ��õ�ϰ�, UV�� ������ ���Ӱ� �����ϴ°���.
        //aiProcess_FindInstances | //�ʹ� �Ž��� ������ Ű����. �����ٴ°Ű���. ���� ������ �Ž����� �ϳ��� ���Ĺ����� ����ε�.
        //aiProcess_OptimizeMeshes |// �Ž� �� �ٿ��ִ� ����ȭ �ɼ��ε�. aiProcess_OptimizeGraph�� ���� ���°� ����, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.�� ȣȯ��.
        //�� ��Ű�°� ������. ���� �ؿ� �ɼ��̶� ȣȯ�Ǵ� ����ε�, �ؿ� �ɼ��� ����.
        //aiProcess_OptimizeGraph |//�ʿ���� ��带 ������. ��尡 �±׷� ���϶� �����Ǵ� ������ �ճ���, ��Ű�°� ������. ���������� �սǵȴٰ� ��.
        aiProcess_TransformUVCoords | //UV�� ���ؼ� ��ȯó�� �Ѵٰ� �ϴ°Ű���. �ؽ��� �̻������� ����������
        aiProcess_JoinIdenticalVertices// �ߺ����� �� �ε��� ���� ������� ��ȯ
        //aiProcess_SortByPType // �������� Ÿ�Ժ��� ��������. aiProcess_Triangulate ���� ������ �ﰢ���� ���Ƽ� �ʿ� ����. �ϴ� �־�~ 
        | aiProcess_GlobalScale;
    if (!xFlip)
    {
        flag |= aiProcess_MakeLeftHanded;
        scene = importer->ReadFile(std::to_string(path + L"\0"), flag);
    }
    else
    {
        scene = importer->ReadFile(std::to_string(path + L"\0"), flag);
        MakeLeftHandedProcess leftHandedProcess;
        leftHandedProcess.Execute(const_cast<aiScene*>(scene));
    }
	assert(scene != nullptr);
}

std::shared_ptr<GameObject> Model::CreateGameObject(const std::shared_ptr<Scene>& scene)
{
	return _rootNode->CreateGameObject(scene, nullptr);
}

void Model::Init(const wstring& path, VertexType vertexType)
{
	std::shared_ptr<AssimpPack> pack = std::make_shared<AssimpPack>();
	pack->Init(path);
	const aiScene* scene = pack->GetScene();

	// �ӽ�
	auto model = GetCast<Model>();
	// -------- �Ž� ���� --------
	_modelMeshList.reserve(scene->mNumMeshes);
	for (int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* currentAIMesh = scene->mMeshes[i];
		std::shared_ptr<ModelMesh> currentModelMesh = std::make_shared<ModelMesh>();
		std::shared_ptr<Mesh> currentMesh = std::make_shared<Mesh>();

		std::vector<uint32_t> indexList;
		LoadIndex(currentAIMesh, indexList);

		switch (vertexType)
		{
			case VertexType::Vertex_Skinned:
			{
				LoadVertex(currentAIMesh, currentModelMesh->skinnedMeshList);
				LoadBone(currentAIMesh, currentModelMesh);
				currentMesh->Init(currentModelMesh->skinnedMeshList, indexList);
				break;
			}
			case VertexType::Vertex_Static:
			{
				LoadVertex(currentAIMesh, currentModelMesh->staticMeshList);
				currentMesh->Init(currentModelMesh->staticMeshList, indexList);
				break;
			}
		}
		currentModelMesh->SetType(vertexType);
		currentModelMesh->SetMesh(currentMesh);
		currentModelMesh->SetIndex(i);
		_modelMeshList.push_back(currentModelMesh);
	}

	_rootNode = AddNode(scene->mRootNode);

	for (auto& bone : _modelBoneList)
		FindNodeByName(bone->GetNodeName())->AddBoneIndex(bone->GetIndex());

	SetDataBone();
}

void Model::DebugLog()
{

}


template <>
void Model::LoadVertex<Vertex_Skinned>(aiMesh* assimp_mesh, std::vector<Vertex_Skinned>& vertexs)
{
	auto mesh2 = std::make_shared<Mesh>();

	vertexs.reserve(assimp_mesh->mNumVertices);
	Vertex_Skinned vert;
	for (int j = 0; j < assimp_mesh->mNumVertices; j++)
	{
		vert = {};
		vert.position = convert_assimp::Format(assimp_mesh->mVertices[j]);
		if (assimp_mesh->HasNormals())
			vert.normal = convert_assimp::Format(assimp_mesh->mNormals[j]);
		if (assimp_mesh->HasVertexColors(0))
			vert.color = convert_assimp::Format(assimp_mesh->mColors[0][j]);
		if (assimp_mesh->HasTangentsAndBitangents())
			vert.tangent = convert_assimp::Format(assimp_mesh->mTangents[j]);
		for (int k = 0; k < 3; k++)
			if (assimp_mesh->HasTextureCoords(k))
				vert.uvs[k] = vec2(convert_assimp::Format(assimp_mesh->mTextureCoords[k][j]));
		vert.boneWeight = Vector4::Zero;
		vert.boneId = Vector4(-1, -1, -1, -1);
		vertexs.push_back(vert);
	}
}

template <>
void Model::LoadVertex<Vertex_Static>(aiMesh* assimp_mesh, std::vector<Vertex_Static>& vertexs)
{
	auto mesh2 = std::make_shared<Mesh>();
	vertexs.reserve(assimp_mesh->mNumVertices);
	Vertex_Static vert;
	for (int j = 0; j < assimp_mesh->mNumVertices; j++)
	{
		vert = {};
		vert.position = convert_assimp::Format(assimp_mesh->mVertices[j]);
		if (assimp_mesh->HasNormals())
			vert.normal = convert_assimp::Format(assimp_mesh->mNormals[j]);
		if (assimp_mesh->HasTextureCoords(0))
			vert.uv = vec2(convert_assimp::Format(assimp_mesh->mTextureCoords[0][j]));
		vertexs.push_back(vert);
	}
}

void Model::LoadIndex(aiMesh* assimp_mesh, std::vector<uint32_t>& indexs)
{
	indexs.reserve(assimp_mesh->mNumFaces * 3);

	for (int j = 0; j < assimp_mesh->mNumFaces; j++)
	{
		auto& polygon = assimp_mesh->mFaces[j];
		if (polygon.mNumIndices == 3)
		{
			indexs.push_back(polygon.mIndices[0]);
			indexs.push_back(polygon.mIndices[1]);
			indexs.push_back(polygon.mIndices[2]);
		}
	}
}

void Model::LoadBone(aiMesh* currentAIMesh, const std::shared_ptr<ModelMesh>& currentModelMesh)
{
	std::vector<Vertex_Skinned>& vertexs = currentModelMesh->skinnedMeshList;

	for (int boneIndex = 0; boneIndex < currentAIMesh->mNumBones; boneIndex++)
	{
		aiBone* currentAIBone = currentAIMesh->mBones[boneIndex];
		std::string boneName = convert_assimp::Format(currentAIBone->mName);
		std::string nodeName = boneName;
		aiNode* boneNode = currentAIBone->mNode;
		if (boneNode != nullptr)
			nodeName = convert_assimp::Format(currentAIBone->mNode->mName);


		auto bone = FindBoneByName(boneName);
		if (bone == nullptr)
		{
			bone = std::make_shared<Bone>();
			bone->SetName(boneName);
			bone->SetNodeName(nodeName);
			bone->SetTransformMatrix(convert_assimp::Format(currentAIBone->mOffsetMatrix));
			AddBone(bone);
		}

		for (int boneVertexIndex = 0; boneVertexIndex < currentAIBone->mNumWeights; boneVertexIndex++)
		{
			auto& currentVertex = vertexs[currentAIBone->mWeights[boneVertexIndex].mVertexId];
			int findIndex = -1;
			float* idArray = &currentVertex.boneId.x;
			float* weightArray = &currentVertex.boneWeight.x;

			const int MAX_BONE_COUNT = 4;

			// ���� �Ⱦ��� ID�� �ִ��� ����
			for (int l = MAX_BONE_COUNT - 1; l >= 0; --l)
				if (idArray[l] == -1)
					findIndex = l;
			// ���� ������ ����
			if (findIndex == -1)
			{
				float minW = currentAIBone->mWeights[boneVertexIndex].mWeight;
				for (int l = 0; l < MAX_BONE_COUNT; l++)
				{
					if (weightArray[l] < minW)
					{
						minW = weightArray[l];
						findIndex = l;
					}
				}
			}
			// ���� ������ ����.
			if (findIndex != -1)
			{
				(&currentVertex.boneId.x)[findIndex] = static_cast<float>(bone->GetIndex());
				(&currentVertex.boneWeight.x)[findIndex] = currentAIBone->mWeights[boneVertexIndex].mWeight;
			}
		}
	}
	for (int vertexIndex = 0; vertexIndex < vertexs.size(); vertexIndex++)
	{
		auto& currentVertex = vertexs[vertexIndex];
		if (currentVertex.boneWeight.LengthSquared() > 1)
			currentVertex.boneWeight.Normalize();
	}
	
}

void Model::SetDataBone()
{
	boneCBuffer = Core::main->GetBufferManager()->GetBufferPool_Static(BufferType::BoneParam)->Alloc(1);
	int offset = 0;
	for (auto& bone : _modelBoneList) {
		memcpy(static_cast<char*>(boneCBuffer->ptr) + offset, &bone->GetTransformMatrix(), sizeof(Matrix));
		offset += sizeof(Matrix);
	}
}

void Model::AddBone(const std::shared_ptr<Bone>& bone)
{
	int boneId = AllocBoneID();
	if (_modelBoneList.size() <= boneId)
		_modelBoneList.resize(boneId+1);
	_modelBoneList[boneId] = bone;
	_nameToBoneTable[bone->GetName()] = bone;
	bone->SetIndex(boneId);
}

std::shared_ptr<ModelNode> Model::AddNode(aiNode* rootNode)
{
	auto currentNode = std::make_shared<ModelNode>();
	currentNode->Init(GetCast<Model>(), rootNode);
	_modelNodeList.push_back(currentNode);
	
	for (int i = 0; i < rootNode->mNumChildren; i++) {
		AddNode(rootNode->mChildren[i])->SetParent(currentNode);
	}

	return currentNode;
}