#pragma once
#include "Component.h"
#include "NavMeshManager.h"

class NavMesh : public Component
{
public:
	~NavMesh() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current) override;
	void SetDestroy() override;
	void Destroy() override;


	std::vector<NavMeshData>& GetNavMeshData();
	void SetNavMeshData(const std::vector<NavMeshData>& data);

	std::vector<NavMeshData> _datas;
	Vector3 startPos;
};

