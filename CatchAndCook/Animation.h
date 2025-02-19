#pragma once


class AnimationPartition;

class Animation
{
public:
	Animation();
	virtual ~Animation();

	void SetName(const wstring& name){
		_name = name;
	}
	wstring& GetName(){
		return _name;
	}
	wstring _name;

	void Init(aiAnimation* anim, aiNode* root);

	std::unordered_map<wstring, std::shared_ptr<AnimationPartition>> _partitions;

};

