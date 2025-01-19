#include "pch.h"
#include "Model.h"

#include "Mesh.h"


AssimpPack::AssimpPack()
{
}

AssimpPack::~AssimpPack()
{
}

void AssimpPack::Init(std::wstring path, bool xFlip)
{
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


void ModelNode::Init(shared_ptr<Model> model, aiNode* node)
{
	_model = model;
	std::string name = convert_assimp::Format(node->mName);
	SetName(name);
	SetLocalSRT(convert_assimp::Format(node->mTransformation));

	_meshIndexList.reserve(node->mNumMeshes);
	for (int i = 0; i < node->mNumMeshes; i++)
		_meshIndexList.push_back(node->mMeshes[i]);
	std::vector<std::shared_ptr<Mesh>> meshList;
	for (auto& meshIndex : _meshIndexList)
		meshList.push_back(model->_meshes[meshIndex]);

	model->_nameToObjectTable[name] = GetCast<ModelNode>();
	model->_nameToMeshsTable[name] = meshList;

}

void ModelNode::SetParent(const std::shared_ptr<ModelNode>& object)
{
	if (object != nullptr)
	{
		_parent = object;
		object->AddChild(GetCast<ModelNode>());
	}
}

void ModelNode::AddChild(const std::shared_ptr<ModelNode>& object)
{
	_childs.push_back(object);
}
