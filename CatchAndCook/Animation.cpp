#include "pch.h"
#include "Animation.h"

#include "AnimationPartition.h"


Animation::Animation()
{
}

Animation::~Animation()
{
}

void Animation::Init(aiAnimation* anim)
{
	SetName(to_wstring(convert_assimp::Format(anim->mName)));
	for(int i=0;i<anim->mNumChannels;i++)
	{
		auto name = to_wstring(convert_assimp::Format(anim->mChannels[i]->mNodeName));
		if(name.find(L"$AssimpFbx$") != std::string::npos)
			continue;
		auto& part = _partitions[name] = make_shared<AnimationPartition>();
		part->Init(anim->mChannels[i]);
	}
}
