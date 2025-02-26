#include "pch.h"
#include "SkinnedHierarchy.h"

#include "Animation.h"
#include "AnimationNode.h"
#include "Bone.h"
#include "Gizmo.h"
#include "ModelNode.h"
#include "SkinnedMeshRenderer.h"
#include "Transform.h"

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
		renderer->AddStructuredSetter(GetCast<SkinnedHierarchy>(), BufferType::BoneParam);
	//SetAnimation(GetOwner()->GetComponentsWithChilds<SkinnedMeshRenderer>()[0]->_model->_animationList[0]);
}

void SkinnedHierarchy::Update()
{
	Component::Update();
	//auto name = to_wstring(GetOwner()->GetComponentWithChilds<SkinnedMeshRenderer>()->_model->_rootBoneNode->GetOriginalName());
	/*
	if (false && animation->_nodeTables.contains(name))
	{
		auto pos = animation->_nodeTables[name]->CalculatePosition(animation->CalculateTime(Time::main->GetTime()));
		//GetOwner()->_transform->SetWorldPosition(Vector3(pos.x, 0, pos.z));
	};
	*/
}

void SkinnedHierarchy::Update2()
{
	Component::Update2();
	Animate(animation, Time::main->GetTime());
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
		renderer->RemoveStructuredSetter(GetCast<SkinnedHierarchy>());
}

void SkinnedHierarchy::SetBoneList(const std::vector<std::shared_ptr<Bone>>& bones)
{
	for (int i = 0; i < bones.size(); i++)
	{
		auto& bone = bones[i];
		_boneNameList.push_back(to_wstring(bone->GetName()));
		_boneOffsetMatrixList[i] = bone->GetTransformMatrix();
	}
}

void SkinnedHierarchy::SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes)
{
	for(auto& name : nodes)
		_nodeNameList.push_back(to_wstring(name->GetName()));
}

void SkinnedHierarchy::SetModel(const std::shared_ptr<Model>& model)
{
	SetNodeList(model->GetNodeList());
	SetBoneList(model->GetBoneList());
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

void SkinnedHierarchy::SetData(StructuredBuffer* buffer)
{
	//int index = buffer->GetShader()->GetRegisterIndex("BoneParam");
	//if(index != -1)
	//	Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _boneCBuffer->GPUAdress);
}

void SkinnedHierarchy::SetAnimation(const std::shared_ptr<Animation>& animation)
{
	this->animation = animation;
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
			nodeObjectTable[name] = obj[0];
			nodeObjectList.push_back(obj[0]);
		}
	}
	for (auto& node : nodeObjectList)
		if (node.lock() != nullptr && node.lock()->GetParent() != nullptr)
			if (node.lock()->GetParent() == GetOwner())
				_rootBone = node;
}

void SkinnedHierarchy::Animate(const std::shared_ptr<Animation>& animation, double time)
{
	if (animation == nullptr)
		return;
	auto finalTime = animation->CalculateTime(time);
	for (auto& animNode : animation->_nodeLists)
	{
		auto it = nodeObjectTable.find(animNode->GetNodeName());
		if (it != nodeObjectTable.end())
		{
			auto obj = it->second.lock();
			if (obj != nullptr) {
				auto transform = obj->_transform;
				auto matrix = animNode->CalculateTransformMatrix(finalTime,animation->_isApplyTransform && animNode->IsRoot());
				transform->SetLocalSRTMatrix(matrix);
			}
		}
	}
}