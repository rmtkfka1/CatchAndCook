#pragma once

struct RayHit
{
	Vector3 normal;
	Vector3 worldPos;
	float distance = 1000000;
	Collider* collider;
	GameObject* gameObject;
	bool isHit = false;

	explicit operator bool() const 
	{
		return isHit;
	}
};

struct PositionHash
{
	size_t operator()(const vec3& v) const
	{
		return std::hash<int>()(static_cast<int>(v.x)) ^
			std::hash<int>()(static_cast<int>(v.y)) ^
			std::hash<int>()(static_cast<int>(v.z));
	}
};


class ColliderManager
{
public:
	static unique_ptr<ColliderManager> main;

	void AddCollider(const std::shared_ptr<Collider>& collider);
	void AddColliderForRay(const std::shared_ptr<Collider>& collider);
	void RemoveAColliderForRay(const std::shared_ptr<Collider>& collider);
	void RemoveCollider(const std::shared_ptr<Collider>& collider);
	void Update();

	bool IsCollision(const std::shared_ptr<Collider>& src);
	bool IsCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest);
	std::unordered_set<std::shared_ptr<Collider>>& GetCollisionList(const std::shared_ptr<Collider>& src);

private:
	void CallBackBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);
	void CallBackEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);
	bool TotalCheckCollision(const std::shared_ptr<Collider>& a,const std::shared_ptr<Collider>& b);
	void VisualizeOccupiedCells(const shared_ptr<Collider>& collider);

public:
	RayHit RayCast(const Ray& ray,const float& dis) const;
	
public:
	float _cellSize = 40.0f;

	vec3 GetGridCell(const vec3& position) const;
	vector<vec3> GetOccupiedCells(const shared_ptr<Collider>& collider) const;
	std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetPotentialCollisions(const std::shared_ptr<Collider>& collider) const;

	std::unordered_map<vec3, std::vector<std::shared_ptr<Collider>>, PositionHash> _staticColliderGrids;
	std::unordered_map<vec3, std::vector<std::shared_ptr<Collider>>, PositionHash> _dynamicColliderGrids;
	std::unordered_map<std::shared_ptr<Collider>,std::unordered_set<std::shared_ptr<Collider>>> _colliderLinkTable;

private:
	vector<std::shared_ptr<Collider>> _collidersForRay;

};


