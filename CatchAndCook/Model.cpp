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

    unsigned int flag = //aiProcess_MakeLeftHanded | // 왼손 좌표계로 변경
        //aiProcess_ConvertToLeftHanded |
        aiProcess_FlipWindingOrder | //CW, CCW 바꾸는거임.
        aiProcess_FlipUVs | // 말그대로 uv의 y축을 뒤집음. 그리고 bitangent도 뒤집음.
        aiProcess_Triangulate | // 4각형 5각형을 3각형으로
        //aiProcess_GenSmoothNormals | // Normal이 없으면 Smmoth Normal 생성
        aiProcess_GenNormals | // Normal이 없으면 Normal 생성
        //aiProcess_ImproveCacheLocality | // 삼각형 개선. 잘 되면 켜보기 캐시히트율을 위해 삼각형 재정렬함.
        //aiProcess_GenUVCoords | // UV없으면 UV 계산하게[ 시키기
        aiProcess_CalcTangentSpace | // 탄젠트 계산
        //aiProcess_SplitLargeMeshes |// 매쉬가 너무 클때 쪼개는거 매쉬 클때 렌더링 유리.
        //aiProcess_Debone | 손실없이 뼈 제거. 걍 쓰지말자.
        //aiProcess_RemoveComponent | // (animations, materials, light sources, cameras, textures, vertex components 제거
        //aiProcess_PreTransformVertices | // root Node를 제외한 모든 하위 노드들 전부 평탄화. 계층 제거.
        //aiProcess_ValidateDataStructure | // 연결 유효성 검사
        //aiProcess_RemoveRedundantMaterials | // 중복이나 안쓰는거 제거
        //aiProcess_FixInfacingNormals | //잘못 연결되서 고장난 노멀 재대로 수정
        //aiProcess_FindDegenerates | //삼각형에서 점이 겹쳐버리면 라인이나 점이 되버리는데, 이걸 Line이나 Point로 변환하는거임. 안쓰는게 나음.
        //aiProcess_FindInvalidData | //유효하지 않는 데이터 감지후 수정, 법선 UV를 제거함. 이렇게 제거하고 나면 aiProcess_GenNormals같은게 새롭게 생성해줄거임. 애니메이션에서도 이점이 있다고함.
        //aiProcess_GenUVCoords  | //UV를 자체적으로 계산함. 모델링툴에서 생성하는걸 추천하고, UV가 없으면 새롭게 생성하는거임.
        //aiProcess_FindInstances | //너무 매쉬가 많을때 키나봄. 느리다는거같음. 같은 재질인 매쉬들을 하나로 합쳐버리는 기능인듯.
        //aiProcess_OptimizeMeshes |// 매쉬 를 줄여주는 최적화 옵션인듯. aiProcess_OptimizeGraph랑 같이 쓰는게 좋고, #aiProcess_SplitLargeMeshes and #aiProcess_SortByPType.랑 호환됨.
        //걍 안키는게 나을듯. 뭔가 밑에 옵션이랑 호환되는 모양인데, 밑에 옵션을 못씀.
        //aiProcess_OptimizeGraph |//필요없는 노드를 삭제함. 노드가 태그로 쓰일때 누락되는 문제가 잇나봄, 안키는게 나을듯. 계층구조가 손실된다고 함.
        aiProcess_TransformUVCoords | //UV에 대해서 변환처리 한다고 하는거같음. 텍스쳐 이상해지면 꺼버리도록
        aiProcess_JoinIdenticalVertices// 중복제거 후 인덱스 버퍼 기반으로 변환
        //aiProcess_SortByPType // 폴리곤을 타입별로 재정렬함. aiProcess_Triangulate 쓰면 어차피 삼각형만 남아서 필요 없음. 일단 넣어~ 
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

	// 임시
	auto model = GetCast<Model>();
	// -------- 매쉬 생성 --------
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

			// 먼저 안쓰는 ID가 있는지 수색
			for (int l = MAX_BONE_COUNT - 1; l >= 0; --l)
				if (idArray[l] == -1)
					findIndex = l;
			// 가장 작은거 수색
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
			// 수색 성공시 갱신.
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