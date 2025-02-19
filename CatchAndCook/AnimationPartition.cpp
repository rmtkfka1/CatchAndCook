#include "pch.h"
#include "AnimationPartition.h"

#include <algorithm>

#include "AnimationKeyFrame.h"


AnimationPartition::AnimationPartition()
{

}

AnimationPartition::~AnimationPartition()
{

}

void AnimationPartition::SetTRS(aiNodeAnim* anim)
{
	SetPosition(anim);
	SetRotation(anim);
	SetScale(anim);
}

void AnimationPartition::SetPosition(aiNodeAnim* anim)
{
    AnimationKeyFrame keyFrame;

	if(anim->mNumPositionKeys <= 1)
        return;

    for(int i=0;i<anim->mNumPositionKeys;i++)
    {
        keyFrame._time = anim->mPositionKeys[i].mTime;
        keyFrame.position = vec3(anim->mPositionKeys[i].mValue.x,anim->mPositionKeys[i].mValue.y,anim->mPositionKeys[i].mValue.z);
		_keyFrame_positions.push_back(keyFrame);
    }
}

void AnimationPartition::SetRotation(aiNodeAnim* anim)
{
    AnimationKeyFrame keyFrame;

    if(anim->mNumRotationKeys <= 1)
        return;

    for(int i = 0; i < anim->mNumRotationKeys; i++)
    {
        keyFrame._time = anim->mRotationKeys[i].mTime;
        keyFrame.rotation = Quaternion(anim->mRotationKeys[i].mValue.x,
                                       anim->mRotationKeys[i].mValue.y,
                                       anim->mRotationKeys[i].mValue.z,
                                       anim->mRotationKeys[i].mValue.w);
		_keyFrame_rotations.push_back(keyFrame);
    }
}

void AnimationPartition::SetScale(aiNodeAnim* anim)
{
    AnimationKeyFrame keyFrame;

    if(anim->mNumScalingKeys <= 1)
        return;

    for(int i = 0; i < anim->mNumScalingKeys; i++)
    {
        keyFrame._time = anim->mScalingKeys[i].mTime;
        keyFrame.scale = vec3(anim->mScalingKeys[i].mValue.x,
                              anim->mScalingKeys[i].mValue.y,
                              anim->mScalingKeys[i].mValue.z);
		_keyFrame_scales.push_back(keyFrame);
    }
}

void AnimationPartition::SetOffsetPosition(const Vector3& position)
{
	offsetPosition = position;
}

void AnimationPartition::SetOffsetScale(const Vector3& scale)
{
	offsetScale = scale;
}

void AnimationPartition::SetOffsetPreRotation(const Quaternion& q)
{
	offsetPreRotation = q;
}

void AnimationPartition::SetOffsetPostRotation(const Quaternion& q)
{
	offsetPostRotation = q;
}

int AnimationPartition::FindKeyFrameIndex(const vector<AnimationKeyFrame>& keyFrames, const double& time)
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

Matrix AnimationPartition::CalculateTransformMatrix(const double& time) const
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
            interpolatedRotation = interpolatedRotation;
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

    return Matrix::CreateScale(interpolatedScale) *
        Matrix::CreateFromQuaternion(offsetPostRotation * interpolatedRotation * offsetPreRotation) *
        Matrix::CreateTranslation(interpolatedPosition);
}
