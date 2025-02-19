#pragma once


class AnimationKeyFrame;

class AnimationNode
{
public:
	AnimationNode();
	virtual ~AnimationNode();

	void SetKeyFrames(aiAnimation* anim, aiNodeAnim* animNode);
	void SetPosition(aiAnimation* anim, aiNodeAnim* animNode);
	void SetRotation(aiAnimation* anim, aiNodeAnim* animNode);
	void SetScale(aiAnimation* anim, aiNodeAnim* animNode);
	void SetPose(Matrix& matrix);
	void SetOffsetPosition(const Vector3& position);
	void SetOffsetScale(const Vector3& scale);
	void SetOffsetPreRotation(const Quaternion& scale);
	void SetOffsetPostRotation(const Quaternion& scale);

	static int FindKeyFrameIndex(const vector<AnimationKeyFrame>& vec, const double& time);
	Matrix CalculateTransformMatrix(const double& time) const;

	void SetNodeName(const wstring& name){
		_nodeName = name;
	}
	wstring& GetNodeName(){
		return _nodeName;
	}

	wstring _nodeName;

	Vector3 offsetPosition = Vector3::Zero;
	Quaternion offsetRotation = Quaternion::Identity;
	Quaternion offsetPreRotation = Quaternion::Identity;
	Quaternion offsetPostRotation = Quaternion::Identity;
	Vector3 offsetScale = Vector3::One;

	bool hasScale = false;
	bool hasPreRotation = false;
	bool hasPostRotation = false;

	vector<AnimationKeyFrame> _keyFrame_positions;
	vector<AnimationKeyFrame> _keyFrame_rotations;
	vector<AnimationKeyFrame> _keyFrame_scales;
};

inline void AnimationNode::SetPose(Matrix& matrix)
{
	matrix.Decompose(offsetScale,offsetRotation,offsetPosition);
}
