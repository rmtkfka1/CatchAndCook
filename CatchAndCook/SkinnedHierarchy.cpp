#include "pch.h"
#include "SkinnedHierarchy.h"

#include "Animation.h"
#include "AnimationNode.h"
#include "Bone.h"
#include "Gizmo.h"
#include "ModelNode.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"



std::unordered_map<std::string, std::string> SkinnedHierarchy::_boneNameToHumanMappingTable;

SkinnedHierarchy::~SkinnedHierarchy()
{
}

bool SkinnedHierarchy::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void SkinnedHierarchy::Init()
{
	Component::Init();
	for(auto&matrix : _boneOffsetMatrixList)
		matrix = Matrix::Identity;
	for (auto& matrix : _finalMatrixList)
		matrix = Matrix::Identity;
}

void SkinnedHierarchy::Start()
{
	Component::Start();

	FindNodeObjects();

	auto o = _rootBone.lock();

	for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
		renderer->AddCbufferSetter(GetCast<SkinnedHierarchy>());
	//SetAnimation(GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>()[0]->_model->_animationList[0]);
}

void SkinnedHierarchy::Update()
{
	Component::Update();
	//auto name = GetOwner()->GetComponentWithChilds<SkinnedMeshRenderer>()->_model->_rootBoneNode->GetOriginalName();

	//_boneHumanNameTable

	if (animation != nullptr)
	{
		auto currentAnimationTime = animation->CalculateTime(Time::main->GetTime());

		auto name = _model->_rootBoneNode->GetOriginalName();
		if (animation->_nodeTables.contains(name))
		{
			auto pos = animation->_nodeTables[name]->CalculateDeltaPosition(prevAnimationTime, currentAnimationTime);
			pos = Vector3::Transform(pos, GetOwner()->_transform->GetWorldRotation());

			GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + pos);
		}
		prevAnimationTime = animation->CalculateTime(Time::main->GetTime());
	}
}

int selectIndex = -1;

void SkinnedHierarchy::Update2()
{
	Component::Update2();
	auto a = ResourceManager::main->GetResourceMap<Animation>();
	std::vector<std::shared_ptr<Animation>> animations;
	for (auto& b : a) {
		animations.push_back(b.second);
	}
	
	for (int i=0;i<9;i++)
		if (Input::main->GetKeyDown(KeyCode::Num1 + i))
		{
			selectIndex = i;
			break;
		}
	if (Input::main->GetKeyDown(KeyCode::Num0))
		selectIndex = -1;
	if (selectIndex != -1)
		SetAnimation(animations[selectIndex]);
	Animate(GetAnimation(), Time::main->GetTime());
}


void SkinnedHierarchy::Animate(const std::shared_ptr<Animation>& animation, double time)
{
	if (animation == nullptr)
		return;
	auto finalTime = animation->CalculateTime(time);

	for (auto& animNode : animation->_nodeLists)
	{
		auto it = FindByName(animNode->GetNodeName(), nodeObjectTable);
		if (it != nodeObjectTable.end())
		{
			auto obj = it->second.lock();
			
			auto q1 = animNode->CalculateRotation(finalTime);
			auto p1 = animNode->CalculatePosition(finalTime);
			auto s1 = animNode->CalculateScale(finalTime);
			if (animNode->GetNodeName() == _model->_rootBoneNode->GetOriginalName())
				std::cout << animNode->GetNodeName() << "\n";

			bool finalRoot = animNode->IsRoot();
			if (animation->_isApplyTransform && ((animNode->GetNodeName() == _model->_rootBoneNode->GetOriginalName())))
				p1 = Vector3(0, p1.y, 0);

			Matrix finalMatrix = Matrix::CreateScale(s1) *
				Matrix::CreateFromQuaternion(q1 * animNode->offsetPreRotation) *
				Matrix::CreateTranslation(p1);
			obj->_transform->SetLocalSRTMatrix(finalMatrix);
		}
	}


	/*
	if (animation == nullptr)
		return;
	auto finalTime = animation->CalculateTime(time);
	for (auto& animNode : animation->_nodeLists)
	{

		//auto it = nodeObjectTable.find(animNode->GetNodeName());
		auto it = FindByName(animNode->GetNodeName(), nodeObjectTable);
		auto it2 = FindByName(animNode->GetNodeName(), _model->_nameToOriginalNodeTable);
		auto it3 = FindByName(animNode->GetNodeName(), animNode->_animation.lock()->_model.lock()->_nameToOriginalNodeTable);
		if (it != nodeObjectTable.end())
		{
			auto obj = it->second.lock();
			std::shared_ptr<ModelNode> currentModelNode;
			std::shared_ptr<ModelNode> animModelNode;
			if (it2 != _model->_nameToOriginalNodeTable.end())
				currentModelNode = it2->second;
			if (it3 != animNode->_animation.lock()->_model.lock()->_nameToOriginalNodeTable.end())
				animModelNode = it3->second;
			
			if (obj != nullptr) {
				auto transform = obj->_transform;
				auto matrix = animNode->CalculateTransformMatrixMapping(currentModelNode, animModelNode, finalTime, animation->_isApplyTransform && animNode->IsRoot());

				animNode->CalculatePosition(finalTime);

				transform->SetLocalSRTMatrix(matrix);
			}
		}
	}
	*/
}

void SkinnedHierarchy::IsPlay(const std::shared_ptr<Animation>& animation, bool isLoop)
{
	_isPlaying = true;
	this->_isLoop = isLoop;
}

void SkinnedHierarchy::IsPlay()
{
	_isPlaying = true;
}


void SkinnedHierarchy::Enable()
{
	Component::Enable();
}

void SkinnedHierarchy::Disable()
{
	Component::Disable();
}

void SkinnedHierarchy::RenderBegin()
{
	Component::RenderBegin();

	GetOwner()->_transform->TopDownLocalToWorldUpdate(Matrix::Identity, false);

	int boneCount = _boneNameList.size();
	for (int i = 0; i < boneCount; i++)
	{
		_finalMatrixList[i] = _boneOffsetMatrixList[i];
		if (auto current = _boneNodeList[i].lock()) {
			_finalMatrixList[i] = _finalMatrixList[i] * current->_transform->_localToWorldMatrix;
			_finalMatrixList[i].Invert(_finalInvertMatrixList[i]);
		}
	}
	_boneCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::BoneParam)->Alloc(1);
	memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneMatrixs), _finalMatrixList.data(), sizeof(Matrix) * boneCount);
	memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneInvertMatrixs), _finalInvertMatrixList.data(),sizeof(Matrix) * boneCount);
}

void SkinnedHierarchy::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider,other);
}

void SkinnedHierarchy::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider,other);
}

void SkinnedHierarchy::SetDestroy()
{
	Component::SetDestroy();
}

void SkinnedHierarchy::Destroy()
{
	Component::Destroy();
	for (auto& renderer : GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>())
		renderer->RemoveCbufferSetter(GetCast<SkinnedHierarchy>());
}

void SkinnedHierarchy::SetBoneList(const std::vector<std::shared_ptr<Bone>>& bones)
{
	_boneNameList.clear();
	for (int i = 0; i < bones.size(); i++)
	{
		auto& bone = bones[i];
		_boneNameList.push_back(to_wstring(bone->GetName()));
		_boneOffsetMatrixList[i] = bone->GetTransformMatrix();
	}
}

void SkinnedHierarchy::SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes)
{
	_nodeNameList.clear();
	for(auto& name : nodes)
		_nodeNameList.push_back(to_wstring(name->GetName()));
}

void SkinnedHierarchy::SetModel(const std::shared_ptr<Model>& model)
{
	_model = model;
	SetNodeList(model->GetNodeList());
	SetBoneList(model->GetBoneList());
}

void SkinnedHierarchy::SetMappingTable(const std::unordered_map<std::string, std::string>& table)
{
	_boneHumanNameTable = table;
}

//void SkinnedHierarchy::PushData()
//{
	//GetOwner()->_transform->TopDownLocalToWorldUpdate(Matrix::Identity, false);

	//_boneCBuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::BoneParam)->Alloc(1);
	//int boneCount = _boneNameList.size();
	//for (int i = 0; i < boneCount; i++)
	//{
	//	_finalMatrixList[i] = _boneOffsetMatrixList[i];
	//	if (auto current = _boneNodeList[i].lock()) {
	//		_finalMatrixList[i] = _finalMatrixList[i] * current->_transform->_localToWorldMatrix;
	//		_finalMatrixList[i].Invert(_finalInvertMatrixList[i]);
	//	}
	//}
	//memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneMatrixs), _finalMatrixList.data(), sizeof(Matrix) * boneCount);
	//memcpy(&(static_cast<BoneParam*>(_boneCBuffer->ptr)->boneInvertMatrixs), _finalInvertMatrixList.data(),sizeof(Matrix) * boneCount);
//}

void SkinnedHierarchy::SetData(Material* material)
{
	int index = material->GetShader()->GetRegisterIndex("BoneParam");
	if(index != -1)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _boneCBuffer->GPUAdress);
}


void SkinnedHierarchy::SetAnimation(const std::shared_ptr<Animation>& animation)
{
	this->animation = animation;
}

std::shared_ptr<Animation> SkinnedHierarchy::GetAnimation()
{
	return animation;
}

void SkinnedHierarchy::FindNodeObjects()
{
	//�̸����� ��Ī�ؼ� ã�ƿ���
	std::vector<std::shared_ptr<GameObject>> obj;
	for (int index = 0; index < _boneNameList.size(); index++)
	{
		auto& boneName = _boneNameList[index];
		{
			obj.clear();
			GetOwner()->GetChildsAllByName(boneName, obj);
			if (!obj.empty())
				_boneNodeList[index] = obj[0];
		}
	}
	for (int index = 0; index < _nodeNameList.size(); index++)
	{
		auto name = _nodeNameList[index];
		obj.clear();
		GetOwner()->GetChildsAllByName(name, obj);
		if (!obj.empty())
		{
			nodeObjectTable[to_string(name)] = obj[0];
			nodeObjectList.push_back(obj[0]);
		}
	}
	for (auto& node : nodeObjectList)
		if (node.lock() != nullptr && node.lock()->GetParent() != nullptr)
			if (node.lock()->GetParent() == GetOwner())
				_rootBone = node;
}
