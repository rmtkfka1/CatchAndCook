#pragma once


class AnimationNode;

class Animation
{
public:
	Animation();
	virtual ~Animation();

	void Init(aiAnimation* anim,aiNode* root);

	double CalculateTime(double time) const;

	void SetName(const wstring& name){
		_name = name;
	}
	wstring& GetName(){
		return _name;
	}
	wstring _name;

	double _ticksPerSecond;
	double _duration;
	double _totalTime;

	bool _isLoop = false;
	bool _isApplyTransform = false;

	std::unordered_map<wstring, std::shared_ptr<AnimationNode>> _nodeTables;
	std::vector<std::shared_ptr<AnimationNode>> _nodeLists;
};

