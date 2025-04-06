#pragma once

enum class CollisionType;
union BoundingUnion;

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
	void UpdateDynamicCells();

	bool CollisionCheckDirect(CollisionType type, BoundingUnion bound);
	bool CollisionCheckDirect(CollisionType type, BoundingUnion bound, std::shared_ptr<Collider>& collider);
	bool CollisionChecksDirect(CollisionType type, BoundingUnion bound, std::vector<std::shared_ptr<Collider>>& colliders);

	bool IsCollision(const std::shared_ptr<Collider>& src);
	bool IsCollision(const std::shared_ptr<Collider>& src, const std::shared_ptr<Collider>& dest);
	std::unordered_set<std::shared_ptr<Collider>>& GetCollisionList(const std::shared_ptr<Collider>& src);

private:
	void CallBackBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	void CallBackEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other);
	bool TotalCheckCollision(const std::shared_ptr<Collider>& a, const std::shared_ptr<Collider>& b);
	void VisualizeOccupiedCells(const vec3& cell , const shared_ptr<Collider>& collider);

public:
	RayHit RayCast(const Ray& ray, const float& dis) const;
	bool RayCastAll(const Ray& ray, const float& dis, std::vector<RayHit>& hitList) const;

	static bool RayCastAll(const std::vector<std::shared_ptr<Collider>>& colliders, const Ray& ray, const float& dis, std::vector<RayHit>& hitList);
	RayHit RayCast(const Ray& ray, const float& dis, shared_ptr<GameObject>& owner) const;
	RayHit RayCastForMyCell(const Ray& ray, const float& dis, shared_ptr<GameObject>& owner) ;

public:
	float _cellSize = 100.0f;

	vec3 GetGridCell(const vec3& position) const;
	vector<vec3> GetOccupiedCells(const shared_ptr<Collider>& collider) const;
	vector<vec3> GetOccupiedCellsDirect(CollisionType type, BoundingUnion bound) const;
	std::unordered_set<std::shared_ptr<Collider>> ColliderManager::GetPotentialCollisions(std::shared_ptr<Collider>& collider);
	std::unordered_set<std::shared_ptr<Collider>> GetPotentialCollisionsDirect(const vector<vec3>& vec);

	std::unordered_map<vec3, std::vector<std::shared_ptr<Collider>>, PositionHash> _staticColliderGrids;
	std::unordered_map<vec3, std::vector<std::shared_ptr<Collider>>, PositionHash> _dynamicColliderGrids;
	std::unordered_map<std::shared_ptr<Collider>, vector<vec3>> _dynamicColliderCashing;
	std::vector<std::shared_ptr<Collider>> _dynamicColliderList;
	std::unordered_map<std::shared_ptr<Collider>, std::unordered_set<std::shared_ptr<Collider>>> _colliderLinkTable;

private:
	vector<std::shared_ptr<Collider>> _collidersForRay; 

};


