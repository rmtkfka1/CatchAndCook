#include "pch.h"
#include "Model.h"

#include "Animation.h"
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
	xFlip = true;
	if (!xFlip)
    {
        flag |= aiProcess_MakeLeftHanded;
        scene = importer->ReadFile(std::to_string(path + L"\0"), flag);
    }
    else
    {
        scene = importer->ReadFile(std::to_string(path + L"\0"), flag);
    }
	if (scene == nullptr)
	{
		std::cout << "assimp load failed : path not found (" << std::to_string(path) << "\n";
		assert(scene != nullptr);
	}
	if(!xFlip)
	{
	} else
	{
		MakeLeftHandedProcess leftHandedProcess;
		leftHandedProcess.Execute(const_cast<aiScene*>(scene));
	}
	
}

std::shared_ptr<GameObject> Model::CreateGameObject(const std::shared_ptr<Scene>& scene)
{
	return _rootNode->CreateGameObject(scene, nullptr);
}

void Model::Init(const wstring& path, VertexType vertexType)
{
	InitGuid();
	std::shared_ptr<AssimpPack> pack = std::make_shared<AssimpPack>();
	pack->Init(path);
	const aiScene* scene = pack->GetScene();
	//aiMetadata* metadata = scene->mMetaData;
	//for(unsigned int i = 0; i < metadata->mNumProperties; ++i) {
	//	aiString key = metadata->mKeys[i];
	//	aiMetadataEntry entry = metadata->mValues[i];

	//	std::cout << "Key: " << key.C_Str() << " - ";

	//	switch(entry.mType) {
	//	case AI_AISTRING:
	//	std::cout << "Value (String): " << static_cast<aiString*>(entry.mData)->C_Str();
	//	break;
	//	case AI_INT32:
	//	std::cout << "Value (Int32): " << *static_cast<int32_t*>(entry.mData);
	//	break;
	//	case AI_UINT64:
	//	std::cout << "Value (UInt64): " << *static_cast<uint64_t*>(entry.mData);
	//	break;
	//	case AI_FLOAT:
	//	std::cout << "Value (Float): " << *static_cast<float*>(entry.mData);
	//	break;
	//	case AI_DOUBLE:
	//	std::cout << "Value (Double): " << *static_cast<double*>(entry.mData);
	//	break;
	//	case AI_AIVECTOR3D:
	//	{
	//		aiVector3D vec = *static_cast<aiVector3D*>(entry.mData);
	//		std::cout << "Value (Vector3D): (" << vec.x << ", " << vec.y << ", " << vec.z << ")";
	//	}
	//	break;
	//	default:
	//	std::cout << "Unknown type.";
	//	break;
	//	}
	//	std::cout << std::endl;
	//}
	aiMetadata* metadata = scene->mMetaData;
	for(unsigned int i = 0; i < metadata->mNumProperties; ++i) {
		aiString key = metadata->mKeys[i];
		aiMetadataEntry entry = metadata->mValues[i];
		if(string(key.C_Str()) == "OriginalUpAxis")
			if(std::abs(*static_cast<int32_t*>(entry.mData)) != 1)
				std::cout << string("Model Warring : Axis Error. Need Unity Fix Tools.\n") << std::to_string(path) <<"\n";
	}
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
				LoadVertex(currentAIMesh, currentMesh, currentModelMesh->skinnedMeshList);
				LoadBone(currentAIMesh, currentModelMesh);
				currentMesh->Init(currentModelMesh->skinnedMeshList, indexList);
				break;
			}
			case VertexType::Vertex_Static:
			{
				LoadVertex(currentAIMesh, currentMesh, currentModelMesh->staticMeshList);
				currentMesh->Init(currentModelMesh->staticMeshList, indexList);
				break;
			}
		}
		currentModelMesh->SetType(vertexType);
		currentModelMesh->SetMesh(currentMesh);
		currentModelMesh->SetIndex(i);
		_modelMeshList.push_back(currentModelMesh);
	}

	LoadNode(scene->mRootNode);
	for(int i=0; i<scene->mNumAnimations; i++)
		LoadAnimation(scene->mAnimations[i], scene->mRootNode);

	SetNodeData();
	SetBoneData();
}

void Model::DebugLog()
{

}


template <>
void Model::LoadVertex<Vertex_Skinned>(aiMesh* assimp_mesh, std::shared_ptr<Mesh> mesh, std::vector<Vertex_Skinned>& vertexs)
{
	auto mesh2 = std::make_shared<Mesh>();

	vertexs.reserve(assimp_mesh->mNumVertices);
	Vertex_Skinned vert;
	Vector3 min = Vector3(10000, 10000, 10000);
	Vector3 max = -min;
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

		Vector3::Min(min, vert.position, min);
		Vector3::Max(max, vert.position, max);
	}
	mesh->SetBound(BoundingBox((max + min) / 2, (max - min) / 2));
}

template <>
void Model::LoadVertex<Vertex_Static>(aiMesh* assimp_mesh, std::shared_ptr<Mesh> mesh, std::vector<Vertex_Static>& vertexs)
{
	auto mesh2 = std::make_shared<Mesh>();
	vertexs.reserve(assimp_mesh->mNumVertices);
	Vertex_Static vert;
	Vector3 min = Vector3(10000, 10000, 10000);
	Vector3 max = -min;
	for (int j = 0; j < assimp_mesh->mNumVertices; j++)
	{
		vert = {};
		vert.position = convert_assimp::Format(assimp_mesh->mVertices[j]);
		if (assimp_mesh->HasNormals())
			vert.normal = convert_assimp::Format(assimp_mesh->mNormals[j]);
		if (assimp_mesh->HasTextureCoords(0))
			vert.uv = vec2(convert_assimp::Format(assimp_mesh->mTextureCoords[0][j]));
		if(assimp_mesh->HasTangentsAndBitangents())
			vert.tangent = convert_assimp::Format(assimp_mesh->mTangents[j]);
		vertexs.push_back(vert);

		Vector3::Min(min, vert.position, min);
		Vector3::Max(max, vert.position, max);
	}
	mesh->SetBound(BoundingBox((max + min) / 2, (max - min) / 2));
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
			for(int l = 0; l < MAX_BONE_COUNT; ++l)
				if(idArray[l] == -1)
				{
					findIndex = l;
					break;
				}
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

	// Index만 존재하는 케이스에 대한 처리
	for(int boneIndex = 0; boneIndex < currentAIMesh->mNumBones; boneIndex++)
	{
		aiBone* currentAIBone = currentAIMesh->mBones[boneIndex];
		std::string boneName = convert_assimp::Format(currentAIBone->mName);
		std::string nodeName = boneName;
		aiNode* boneNode = currentAIBone->mNode;
		if(boneNode != nullptr)
			nodeName = convert_assimp::Format(currentAIBone->mNode->mName);

		auto bone = FindBoneByName(boneName);
		if(bone == nullptr)
		{
			bone = std::make_shared<Bone>();
			bone->SetName(boneName);
			bone->SetNodeName(nodeName);
			bone->SetTransformMatrix(convert_assimp::Format(currentAIBone->mOffsetMatrix));
			AddBone(bone);
		}

		if(currentAIBone->mNumWeights == 0)
		{
			for(int boneVertexIndex = 0; boneVertexIndex < vertexs.size(); boneVertexIndex++)
			{
				auto& currentVertex = vertexs[boneVertexIndex];
				int findIndex = -1;
				float* idArray = &currentVertex.boneId.x;

				const int MAX_BONE_COUNT = 4;

				// ���� �Ⱦ��� ID�� �ִ��� ����
				for(int l = 0; l < MAX_BONE_COUNT; ++l)
					if(idArray[l] == -1)
					{
						findIndex = l;
						break;
					}
				if((currentVertex.boneId.x + currentVertex.boneId.y + currentVertex.boneId.z + currentVertex.boneId.w == -4) && findIndex != -1)
				{
					(&currentVertex.boneId.x)[findIndex] = static_cast<float>(bone->GetIndex());
					(&currentVertex.boneWeight.x)[findIndex] = 1;
				}
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

void Model::LoadNode(aiNode* root)
{
	_rootNode = AddNode(root);
}

void Model::LoadAnimation(aiAnimation* aiAnim, aiNode* root)
{
	auto anim = std::make_shared<Animation>();
	anim->Init(aiAnim, root);
	_animationList.push_back(anim);
	_nameToAnimationTable[to_string(anim->GetName())] = anim;
	ResourceManager::main->Add(anim->GetName(), anim);
}

void Model::SetNodeData()
{
	for (auto& bone : _modelBoneList)
	{
		auto currentNode = FindNodeByName(bone->GetNodeName());
		currentNode->AddBoneIndex(bone->GetIndex());
		currentNode->SetDynamic(false);
	}
}

void Model::SetBoneData()
{
	//boneCBuffer = Core::main->GetBufferManager()->GetBufferPool_Static(BufferType::BoneParam)->Alloc(1);
	//int offset = 0;
	//for (auto& bone : _modelBoneList) {
	//	memcpy(static_cast<char*>(boneCBuffer->ptr) + offset, &bone->GetTransformMatrix(), sizeof(Matrix));
	//	offset += sizeof(Matrix);
	//}
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
	
	for (int i = 0; i < rootNode->mNumChildren; i++) {
		AddNode(rootNode->mChildren[i])->SetParent(currentNode);
	}

	return currentNode;
}