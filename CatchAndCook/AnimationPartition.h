#pragma once


class AnimationKeyFrame;

class AnimationPartition
{
public:
	AnimationPartition();
	virtual ~AnimationPartition();

	void Init(aiNodeAnim* anim);

	static int FindKeyFrameIndex(const vector<AnimationKeyFrame>& vec, const double& time);
	Matrix CalculateTransformMatrix(const double& time) const;

	void SetNodeName(const wstring& name){
		_nodeName = name;
	}
	wstring& GetNodeName(){
		return _nodeName;
	}
	wstring _nodeName;

	vector<AnimationKeyFrame> _keyFrame_positions;
	vector<AnimationKeyFrame> _keyFrame_rotations;
	vector<AnimationKeyFrame> _keyFrame_scales;
};
