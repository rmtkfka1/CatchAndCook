#pragma once


class AnimationKeyFrame;

class AnimationPartition
{
public:
	AnimationPartition();
	virtual ~AnimationPartition();

	void SetTRS(aiNodeAnim* anim);
	void SetPosition(aiNodeAnim* anim);
	void SetRotation(aiNodeAnim* anim);
	void SetScale(aiNodeAnim* anim);
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

	vector<AnimationKeyFrame> _keyFrame_positions;
	vector<AnimationKeyFrame> _keyFrame_rotations;
	vector<AnimationKeyFrame> _keyFrame_scales;
};

inline void AnimationPartition::SetPose(Matrix& matrix)
{
	matrix.Decompose(offsetScale,offsetRotation,offsetPosition);
}
