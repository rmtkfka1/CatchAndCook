#pragma once

struct RayHit
{
	Vector3 normal;
	Vector3 worldPos;
	float distance = 1000000;
	Collider* collider;
	GameObject* gameObject;
	bool isHit = false;
	explicit operator bool() const {
		return isHit;
	}
};

class ColliderManager
{
public:
	static unique_ptr<ColliderManager> main;

	void AddCollider(const std::shared_ptr<Collider>& collider);
	void RemoveCollider(const std::shared_ptr<Collider>& collider);

	void Update();

	bool TotalCheckCollision(const std::shared_ptr<Collider>& a,const std::shared_ptr<Collider>& b);

	//충돌한게 있는지 체크
	bool IsCollisionStay(const std::shared_ptr<Collider>& a) const;
	bool CollisionStayContains(const std::shared_ptr<Collider>& a,const std::shared_ptr<Collider>& b);
	std::unordered_set<std::shared_ptr<Collider>> GetCollisionStayTable(const std::shared_ptr<Collider>& a);

	RayHit RayCast(const Ray& ray, const float& dis) const;
	void RayCastAll(const Vector3& worldPos,const Vector3& dir,const float& dis, std::vector<RayHit>& hits);

private:
	void CallBackBegin(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);
	void CallBackEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other);

private:

	std::vector<std::shared_ptr<Collider>> _staticColliders;
	std::vector<std::shared_ptr<Collider>> _dynamicColliders;
	std::unordered_map<std::shared_ptr<Collider>, std::unordered_set<std::shared_ptr<Collider>>> _colliderLinkTable;
};


