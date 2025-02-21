#pragma once
#include "Component.h"

class SkinnedHierarchy : public Component, public RenderObjectSetter
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

	void SetBoneList(const std::vector<std::shared_ptr<Bone>>& bones){ _boneList = bones;}
	void SetNodeList(const std::vector<std::shared_ptr<ModelNode>>& nodes);
	void PushData() override;
	void SetData(Material* material) override;

	void SetAnimation(const std::shared_ptr<Animation>& animation);

	std::shared_ptr<Animation> animation;

	std::unordered_map<std::wstring, std::weak_ptr<GameObject>> nodeObjectTable;
	vector<std::weak_ptr<GameObject>> nodeObjectList;

	std::vector<std::shared_ptr<Bone>> _boneList;
	std::vector<wstring> _nodeNameList;


	std::array<Matrix, 256> _boneOffsetMatrixList; 
	std::array<std::weak_ptr<GameObject>, 256> _boneNodeList;

	std::array<Matrix, 256> _finalMatrixList;
	std::array<Matrix,256> _finalInvertMatrixList;

	CBufferContainer* _cbuffer;
};

