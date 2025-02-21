#include "pch.h"
#include "SkinnedHierarchy.h"

#include "Animation.h"
#include "AnimationNode.h"
#include "Bone.h"
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
	for(int index = 0;index<_boneList.size();index++)
	{
		auto& bone = _boneList[index];
		_boneOffsetMatrixList[index] = bone->GetTransformMatrix();
		{
			std::vector<std::shared_ptr<GameObject>> obj;
			auto name = to_wstring(bone->GetName());
			GetOwner()->GetChildsAllByName(name,obj);
			if(!obj.empty())
				_boneNodeList[index] = obj[0];
		}
	}
	//_nodeNameList
	for(int index = 0;index<_nodeNameList.size();index++)
	{
		{
			std::vector<std::shared_ptr<GameObject>> obj;

			auto name = _nodeNameList[index];
			GetOwner()->GetChildsAllByName(name, obj);
			if (!obj.empty())
				nodeObjectTable[name] = obj[0];
			nodeObjectList.push_back(obj.empty() ? nullptr : obj[0]);
		}
	}
	std::vector<std::shared_ptr<SkinnedMeshRenderer>> renderers;
	GetOwner()->GetComponentsWithChilds(renderers);
	for (auto& renderer : renderers)
		renderer->AddSetter(GetCast<SkinnedHierarchy>());

	if(renderers.size() != 0)
		SetAnimation(renderers[0]->_model->_animationList[0]);
}

void SkinnedHierarchy::Update()
{
	Component::Update();
}

void SkinnedHierarchy::Update2()
{
	Component::Update2();

	auto time = this->animation->CalculateTime(Time::main->GetTime());
	for(auto& animNode : this->animation->_nodeLists)
	{
		auto it = nodeObjectTable.find(animNode->GetNodeName());
		if(it != nodeObjectTable.end())
		{
			auto obj = it->second.lock();
			if(obj != nullptr)
			{
				auto transform = obj->_transform;
				auto matrix = animNode->CalculateTransformMatrix(time);
				transform->SetLocalSRTMatrix(matrix);
			}
		}
	}
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
	PushData();
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
}

void SkinnedHierarchy::SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes)
{
	for(auto& name : nodes)
		_nodeNameList.push_back(to_wstring(name->GetName()));
}

void SkinnedHierarchy::PushData()
{
	_cbuffer = Core::main->GetBufferManager()->GetBufferPool(BufferType::BoneParam)->Alloc(1);
	GetOwner()->_transform->TopDownLocalToWorldUpdate(Matrix::Identity, true);
	for (int i = 0; i < _boneList.size(); i++)
	{
		_finalMatrixList[i] = _boneOffsetMatrixList[i];
		if (auto current = _boneNodeList[i].lock()) {
			_finalMatrixList[i] = _finalMatrixList[i] * current->_transform->_localToWorldMatrix;
			_finalMatrixList[i].Invert(_finalInvertMatrixList[i]);
		}
	}
	memcpy(_cbuffer->ptr, _finalMatrixList.data(), sizeof(Matrix) * _boneList.size());
	memcpy(&(static_cast<BoneParam*>(_cbuffer->ptr)->boneInvertMatrixs), _finalInvertMatrixList.data(),sizeof(Matrix) * _boneList.size());
}

void SkinnedHierarchy::SetData(Material* material)
{
	int index = material->GetShader()->GetRegisterIndex("BoneParam");
	if(index != -1)
		Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(index, _cbuffer->GPUAdress);
}

void SkinnedHierarchy::SetAnimation(const std::shared_ptr<Animation>& animation)
{
	this->animation = animation;
}
