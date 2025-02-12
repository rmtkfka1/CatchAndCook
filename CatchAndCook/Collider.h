
#include "ColliderManager.h"
#include "Component.h"

enum class CollisionType
{
	Box,
	Sphere,
	Frustum
};

union BoundingUnion
{
	BoundingOrientedBox box;
	BoundingSphere sphere;
	BoundingFrustum frustum;
};

struct WeakPtrHash
{
	template <typename T>
	size_t operator()(const std::weak_ptr<T>& wp) const
	{
		if(auto sp = wp.lock())
			return std::hash<std::shared_ptr<T>>{}(sp);
		return 0;
	}
};

struct WeakPtrEqual
{
	template <typename T>
	bool operator()(const std::weak_ptr<T>& lhs,const std::weak_ptr<T>& rhs) const
	{
		return lhs.lock() == rhs.lock();
	}
};

class Collider : public Component
{
public:
	Collider();
	~Collider() override;
	bool IsExecuteAble() override;
	void Init() override;
	void Start() override;
	void Update() override;
	void Update2() override;
	void Enable() override;
	void Disable() override;
	void RenderBegin() override;
	void CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other) override;
	void CollisionEnd(const std::shared_ptr<Collider>& collider,const std::shared_ptr<Collider>& other) override;
	void SetDestroy() override;
	void Destroy() override;

public:
	bool IsCollision();
	bool IsCollision(const std::shared_ptr<Collider>& a);

public:
	bool CheckCollision(const std::shared_ptr<Collider>& other);

	bool RayCast(const Ray& ray, const float& dis, RayHit& hit);

	void SetBoundingBox(vec3 center,vec3 extents);
	void SetBoundingSphere(vec3 center,float radius);
	void SetBoundingFrustum(BoundingFrustum& boundingFrustum);
	void CalculateBounding();

	BoundingUnion& GetBoundUnion() {return _bound;}

private:
	friend class ColliderManager;

private:
	CollisionType _type;
	BoundingUnion _orgin;
	BoundingUnion _bound;

	int groupId = 0;

	unordered_set<weak_ptr<Collider>,WeakPtrHash,WeakPtrEqual> _collisionList;
};
