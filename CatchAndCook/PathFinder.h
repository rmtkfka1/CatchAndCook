#pragma once
#include "Component.h"


struct FishPath
{
	vector<vec3> path;
	bool AreyouDraw = false;
};


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
	void SetMoveSpeed(float speed) { _moveSpeed = speed; }
	void SetStartIndex(int index) { _currentIndex = index; }
	void SetPass(const wstring& path);
	static void ClearDebugDraw();
private:
	void ReadPathFile(const std::wstring& fileName);
private:
	static unordered_map<wstring, FishPath> _pathList;

	int _currentIndex = 0;
	float _moveSpeed = 200.0f; 
	float _distanceMoved = 0.0f;
	float _segmentLength = 0.0f;
	wstring _pathName = L"Null";
	Quaternion _firstQuat;

};

