﻿#pragma once
#include "Component.h"

class SkinnedHierarchy : public Component, public RenderCBufferSetter
{
public:
	~SkinnedHierarchy() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

	void SetBoneList(const std::vector<std::shared_ptr<Bone>>& bones);
	void SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes);
	void SetModel(const std::shared_ptr<Model>& model);
	

	void SetAnimation(const std::shared_ptr<Animation>& animation);
	void FindNodeObjects();
	void Animate(const std::shared_ptr<Animation>& animation, double time);
	void SetData(Material* material) override;

	std::shared_ptr<Animation> animation;

	std::unordered_map<std::wstring, std::weak_ptr<GameObject>> nodeObjectTable;

	vector<std::weak_ptr<GameObject>> nodeObjectList;
	std::array<std::weak_ptr<GameObject>, 256> _boneNodeList;
	std::weak_ptr<GameObject> _rootBone;

	std::vector<wstring> _boneNameList;
	std::vector<wstring> _nodeNameList;


	std::array<Matrix, 256> _boneOffsetMatrixList; 

	std::array<Matrix, 256> _finalMatrixList;
	std::array<Matrix, 256> _finalInvertMatrixList;

	CBufferContainer* _boneCBuffer;
};


