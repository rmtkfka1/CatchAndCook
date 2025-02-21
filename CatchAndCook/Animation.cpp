#include "pch.h"
#include "Animation.h"

#include "AnimationNode.h"
#include "AnimationKeyFrame.h"


Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Init(aiAnimation* anim, aiNode* root)
{
	//root
	SetName(to_wstring(convert_assimp::Format(anim->mName)));

	_duration = anim->mDuration;
	_ticksPerSecond = (anim->mTicksPerSecond != 0) ? anim->mTicksPerSecond : 25.0;
	_totalTime = _duration / _ticksPerSecond;

	for(int i=0;i<anim->mNumChannels;i++)
	{
		
		auto name = to_wstring(convert_assimp::Format(anim->mChannels[i]->mNodeName));
		wstring originalName = name;

		if (name.find(L"$AssimpFbx$") != std::string::npos)
		{
			vector<wstring> strList = wstr::split(name,L"_");
			vector<wstring> originalList = wstr::split(name, L"_$AssimpFbx$");
			originalName = originalList[0];
		}
		
		auto part = _nodeTables[originalName];
		if(part == nullptr)
		{
			part = std::make_shared<AnimationNode>();
			part->SetNodeName(originalName);
			_nodeTables[originalName] = part;
			_nodeLists.push_back(part);
		}
		part->SetKeyFrames(anim, anim->mChannels[i]);
	}


	for(auto& part : _nodeTables)
	{
		Vector3 pos,scale;
		Quaternion rot;

		auto findNode = root->FindNode(to_string(part.first+ L"_$AssimpFbx$_Translation").c_str());
		if(findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale,rot,pos);
			part.second->SetOffsetPosition(pos);
		}
		findNode = root->FindNode(to_string(part.first+ L"_$AssimpFbx$_Scaling").c_str());
		if(findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale,rot,pos);
			part.second->SetOffsetScale(scale);
		}
		findNode = root->FindNode(to_string(part.first+ L"_$AssimpFbx$_PreRotation").c_str());
		if(findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale,rot,pos);
			part.second->SetOffsetPreRotation(rot);
		}
		findNode = root->FindNode(to_string(part.first+ L"_$AssimpFbx$_PostRotation").c_str());
		if(findNode != nullptr)
		{
			auto trs = convert_assimp::Format(findNode->mTransformation);
			trs.Decompose(scale,rot,pos);
			part.second->SetOffsetPostRotation(rot);
		}

	}
}

double Animation::CalculateTime(double time) const
{
	if(_totalTime == 0)
		return time;
	return std::fmod(time,_totalTime);
}
