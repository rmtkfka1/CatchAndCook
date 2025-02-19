#include "pch.h"
#include "AnimationNode.h"


#include <algorithm>

#include "AnimationKeyFrame.h"


AnimationNode::AnimationNode()
{

}

AnimationNode::~AnimationNode()
{

}

void AnimationNode::SetKeyFrames(aiAnimation* anim, aiNodeAnim* animNode)
{
	SetPosition(anim, animNode);
	SetRotation(anim, animNode);
	SetScale(anim, animNode);
}

void AnimationNode::SetPosition(aiAnimation* anim, aiNodeAnim* animNode)
{
    AnimationKeyFrame keyFrame;

	if(animNode->mNumPositionKeys <= 1)
        return;

    for(int i=0;i<animNode->mNumPositionKeys;i++)
    {
        keyFrame._tick = animNode->mPositionKeys[i].mTime;
        keyFrame._time = keyFrame._tick / anim->mTicksPerSecond;
        keyFrame.position = vec3(animNode->mPositionKeys[i].mValue.x,animNode->mPositionKeys[i].mValue.y,animNode->mPositionKeys[i].mValue.z);
		_keyFrame_positions.push_back(keyFrame);
    }
}

void AnimationNode::SetRotation(aiAnimation* anim, aiNodeAnim* animNode)
{
    AnimationKeyFrame keyFrame;

    if(animNode->mNumRotationKeys <= 1)
        return;

    for(int i = 0; i < animNode->mNumRotationKeys; i++)
    {
        keyFrame._tick = animNode->mRotationKeys[i].mTime;
        keyFrame._time = keyFrame._tick / anim->mTicksPerSecond;
        keyFrame.rotation = Quaternion(animNode->mRotationKeys[i].mValue.x,
                                       animNode->mRotationKeys[i].mValue.y,
                                       animNode->mRotationKeys[i].mValue.z,
                                       animNode->mRotationKeys[i].mValue.w);
		_keyFrame_rotations.push_back(keyFrame);
    }
}

void AnimationNode::SetScale(aiAnimation* anim, aiNodeAnim* animNode)
{
    AnimationKeyFrame keyFrame;

    if(animNode->mNumScalingKeys <= 1)
        return;

    for(int i = 0; i < animNode->mNumScalingKeys; i++)
    {
        keyFrame._tick = animNode->mScalingKeys[i].mTime;
        keyFrame._time = keyFrame._tick / anim->mTicksPerSecond;
        keyFrame.scale = vec3(animNode->mScalingKeys[i].mValue.x,
                              animNode->mScalingKeys[i].mValue.y,
                              animNode->mScalingKeys[i].mValue.z);
		_keyFrame_scales.push_back(keyFrame);
    }
}

void AnimationNode::SetOffsetPosition(const Vector3& position)
{
	offsetPosition = position;
}

void AnimationNode::SetOffsetScale(const Vector3& scale)
{
	offsetScale = scale;
    hasScale = true;
}

void AnimationNode::SetOffsetPreRotation(const Quaternion& quat)
{
	offsetPreRotation = quat;
    hasPreRotation = true;
}

void AnimationNode::SetOffsetPostRotation(const Quaternion& quat)
{
	offsetPostRotation = quat;
    hasPostRotation = true;
}

int AnimationNode::FindKeyFrameIndex(const vector<AnimationKeyFrame>& keyFrames, const double& time)
{
    if(keyFrames.empty())
        return -1;
    auto it = std::lower_bound(keyFrames.begin(),keyFrames.end(), time,
        [&](const AnimationKeyFrame& key, const double& t) {
        return key._time < t;
    });

    if(it == keyFrames.begin())
        return -1;

    return static_cast<int>(std::distance(keyFrames.begin(), it)) - 1;
}

Matrix AnimationNode::CalculateTransformMatrix(const double& time) const
{
    Vector3 interpolatedPosition = offsetPosition;
    Quaternion interpolatedRotation = offsetRotation;
    Vector3 interpolatedScale = offsetScale;

    {
        const size_t keyCount = _keyFrame_positions.size();
        if(keyCount != 0)
        {
            if(keyCount == 1)
                interpolatedPosition = _keyFrame_positions[0].position;
            int index = FindKeyFrameIndex(_keyFrame_positions,time);
            index = std::max(index,0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_positions[index];
            const auto& key1 = _keyFrame_positions[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.position,key1.position, t,interpolatedPosition);
        }
    }
    {
        const size_t keyCount = _keyFrame_rotations.size();
        if(keyCount != 0)
        {
            if(keyCount == 1)
                interpolatedRotation = _keyFrame_rotations[0].rotation;
            int index = FindKeyFrameIndex(_keyFrame_rotations,time);
            index = std::max(index,0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_rotations[index];
            const auto& key1 = _keyFrame_rotations[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Quaternion::Slerp(key0.rotation,key1.rotation, t,interpolatedRotation);
        }
    }

    {
        const size_t keyCount = _keyFrame_scales.size();
        if(keyCount != 0)
        {
            if(keyCount == 1)
                interpolatedScale = _keyFrame_scales[0].scale;
            int index = FindKeyFrameIndex(_keyFrame_scales,time);
            index = std::max(index,0);
            int nextIndex = (index + 1 < keyCount) ? index + 1 : 0;
            const auto& key0 = _keyFrame_scales[index];
            const auto& key1 = _keyFrame_scales[nextIndex];
            const double dt = key1._time - key0._time;
            const double t = (dt != 0.f) ? (time - key0._time) / dt : 0.f;
            Vector3::Lerp(key0.scale,key1.scale, t,interpolatedScale);
        }
    }

    //hasScale
    Matrix matrix = Matrix::CreateTranslation(interpolatedPosition);

    if(hasPreRotation)
        interpolatedRotation = interpolatedRotation * offsetPreRotation;
    if(hasPostRotation)
        interpolatedRotation = offsetPostRotation * interpolatedRotation;
    matrix = Matrix::CreateFromQuaternion(interpolatedRotation) * matrix;
    if(hasScale)
        matrix = Matrix::CreateScale(interpolatedScale) * matrix;

    return matrix;
}
