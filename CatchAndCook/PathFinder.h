﻿#pragma once
#include "Component.h"
class PathFinder :public Component
{

public:
	PathFinder();
	~PathFinder();
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void RenderBegin() override;
	void Enable() override;
	void Disable() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	bool IsExecuteAble() { return true; };

public:
	void ReadPathFile(const std::wstring& fileName);

private:
	vector<vec3> _paths;

	int currentIndex = 0;
	float moveSpeed = 30.0f; // 초당 거리 (단위: units/sec)
	float distanceMoved = 0.0f;
	float segmentLength = 0.0f;

};

