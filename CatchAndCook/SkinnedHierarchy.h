#pragma once
#include "Component.h"

class SkinnedHierarchy : public Component, public RenderCBufferSetter
{
public:
	static std::unordered_map<std::string, std::string> _boneNameToHumanMappingTable;

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
	void SetMappingTable(const std::unordered_map<std::string, std::string>& table);

	template <class T>
	typename std::unordered_map<std::string, T>::iterator
		FindByName(const std::string& name, std::unordered_map<std::string, T>& data)
	{
		auto it = data.find(name);
		if (it != data.end())
			return it;

		auto it2 = _boneNameToHumanMappingTable.find(name);
		if (it2 != _boneNameToHumanMappingTable.end())
		{
			auto it3 = _boneHumanNameTable.find(it2->second);
			if (it3 != _boneHumanNameTable.end())
				return data.find(it3->second);
		}
		return data.end();
	}

	template <class T>
	std::string FindNameByName(const std::string& name, std::unordered_map<std::string, T>& data)
	{
		auto it = data.find(name);
		if (it != data.end())
			return name;

		auto it2 = _boneNameToHumanMappingTable.find(name);
		if (it2 != _boneNameToHumanMappingTable.end())
		{
			auto it3 = _boneHumanNameTable.find(it2->second);
			if (it3 != _boneHumanNameTable.end())
				return it3->second;
		}
		return"";
	}
	

	void SetAnimation(const std::shared_ptr<Animation>& animation);
	std::shared_ptr<Animation> GetAnimation();
	void FindNodeObjects();
	void Animate(const std::shared_ptr<Animation>& animation, double time);
	void SetData(Material* material) override;

	std::shared_ptr<Model> _model;
	std::shared_ptr<Animation> animation;
	std::shared_ptr<Animation> _nextAnimation;

	std::unordered_map<std::string, std::string> _boneHumanNameTable;

	std::unordered_map<std::string, std::weak_ptr<GameObject>> nodeObjectTable;

	vector<std::weak_ptr<GameObject>> nodeObjectList;
	std::array<std::weak_ptr<GameObject>, 256> _boneNodeList;
	std::weak_ptr<GameObject> _rootBone;

	std::vector<wstring> _boneNameList;
	std::vector<wstring> _nodeNameList;


	std::array<Matrix, 256> _boneOffsetMatrixList; 

	std::array<Matrix, 256> _finalMatrixList;
	std::array<Matrix, 256> _finalInvertMatrixList;

	CBufferContainer* _boneCBuffer;



	float prevAnimationTime = 0;
};


