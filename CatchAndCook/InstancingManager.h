#pragma once

#include "StructuredBuffer.h"

union InstanceID
{
	struct
	{
		uint32 meshID;
		uint32 materialID;
	};

	uint64 id;
};

class InstancingManager
{

public:
	static unique_ptr<InstancingManager> main;
	
public:
	void Render();
	void Clear();
	void AddObject(RenderObjectStrucutre& obj);
	void AddParam(uint64 instanceID , Instance_Transform& data);

private:
	map<uint64,vector<RenderObjectStrucutre>> _objectMap;

};

